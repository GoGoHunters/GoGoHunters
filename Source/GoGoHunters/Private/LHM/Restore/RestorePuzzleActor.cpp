// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/Restore/RestorePuzzleActor.h"
#include "LHM/Restore/PieceActor.h"
#include "LHM/Restore/RestoreManager.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ARestorePuzzleActor::ARestorePuzzleActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));

	RotationBoard = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RotationBoard"));
	RotationBoard->SetupAttachment(RootComponent);
	RotationBoard->SetRelativeScale3D(FVector(0.3f));

}

// Called when the game starts or when spawned
void ARestorePuzzleActor::BeginPlay()
{
	Super::BeginPlay();
	
	UE_LOG(LogTemp, Warning, TEXT("RestorePuzzleActor BeginPlay"));
}

// Called every frame
void ARestorePuzzleActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARestorePuzzleActor::InitPuzzle(const FCRelicData& InRelicData, ARestoreManager* InManager)
{
	UE_LOG(LogTemp, Warning, TEXT("InitPuzzle: %s"), *InRelicData.RelicName.ToString());
	RelicData = InRelicData;
	RestoreManager = InManager;

	// 이전에 생성된 유물이 있으면 제거
	if (SpawnedRelic)
	{
		SpawnedRelic->Destroy();
		SpawnedRelic = nullptr;
	}

	// 우선은 항상 BP_RestoreEgg만 사용
	if (!RelicClass) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// 유물은 회전판 옆 박스에 스폰
	FTransform SpawnTransform = FTransform::Identity;
	SpawnTransform.SetLocation(FVector(256, -7, 0)); // (X=256.000000,Y=-7.000000,Z=0.000000)

	SpawnedRelic = GetWorld()->SpawnActor<AActor>(RelicClass, SpawnTransform, SpawnParams);

#pragma region SnapPointTransforms & PieceActors 저장
	// GuideMesh & CompletedMesh 위치 조정
	/*UStaticMeshComponent* */GuideMesh = Cast<UStaticMeshComponent>(SpawnedRelic->GetComponentsByTag(UStaticMeshComponent::StaticClass(), FName("Guide"))[0]);
	if (GuideMesh)
	{
		GuideMesh->SetWorldLocation(RotationBoard->GetComponentLocation() + FVector(0, 0, 25.5f));
	}

	UStaticMeshComponent* CompletedMesh = Cast<UStaticMeshComponent>(SpawnedRelic->GetComponentsByTag(UStaticMeshComponent::StaticClass(), FName("Complete"))[0]);
	if (CompletedMesh)
	{
		CompletedMesh->SetHiddenInGame(true);
		CompletedMesh->SetWorldLocation(GuideMesh->GetComponentLocation());
	}

	// 조각 목록 저장
	PieceActors.Empty();
	TArray<AActor*> ChildActors;
	SpawnedRelic->GetAllChildActors(ChildActors);

	ChildActors.Sort([](const AActor& A, const AActor& B)
	{
		return A.GetName() < B.GetName();
	});

	for (int32 i = 0; i < ChildActors.Num(); ++i)
	{
		if (APieceActor* Piece = Cast<APieceActor>(ChildActors[i]))
		{
			PieceActors.Add(Piece);
			Piece->SetPieceIndex(i); // 이름 순서대로 인덱스 지정
			UE_LOG(LogTemp, Warning, TEXT("PieceActors(%d): %s"), i, *Piece->GetName());
		}
		else if (ChildActors[i]->ActorHasTag("Guide"))
		{
			TArray<USceneComponent*> GuideChildActors;
			ChildActors[i]->GetRootComponent()->GetChildrenComponents(true, GuideChildActors);

			GuideChildActors.Sort([](const USceneComponent& A, const USceneComponent& B)
							{
								return A.GetName() < B.GetName();
							});

			SnapPoints.Empty();
			for (USceneComponent* Scene : GuideChildActors)
			{
				if (Scene && Scene->ComponentHasTag("SnapPoint"))
				{
					SnapPoints.Add(Scene);
					//UE_LOG(LogTemp, Warning, TEXT("SnapPoints(%d): %s at %s"), SnapPoints.Num() - 1, *Scene->GetName(), *Scene->GetAttachParentActor()->GetName());
				}
			}
		}
	}
#pragma endregion
}

void ARestorePuzzleActor::TrySnapPiece(class APieceActor* Piece)
{
	if (!Piece || !SnapPoints.IsValidIndex(Piece->GetPieceIndex()))
		return;

	const int32 Index = Piece->GetPieceIndex();
	USceneComponent* SnapPoint = SnapPoints[Index];

	const float SnapRadius = 30.f;
	const float Dist = FVector::Dist(SnapPoint->GetComponentLocation(), Piece->GetActorLocation());

	const float GuideRadius = 30.f;
	const float GuideDist = FVector::Dist(GuideMesh->GetComponentLocation(), Piece->GetActorLocation());

	if (Dist < SnapRadius)
	{
		// 1. DetachFromActor
		Piece->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		// 2. 물리 먼저 끄기
		if (UPrimitiveComponent* Primitive = Cast<UPrimitiveComponent>(Piece->GetRootComponent()))
		{
			Primitive->SetSimulatePhysics(false);
			Primitive->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}

		// 3. AttachToComponent
		Piece->SetActorLocationAndRotation(SnapPoint->GetComponentLocation(), SnapPoint->GetComponentRotation());
		bool bAttachSuccess = Piece->AttachToComponent(
			SnapPoint,
			FAttachmentTransformRules::KeepWorldTransform
		);

		UE_LOG(LogTemp, Warning, TEXT("Second Attach Attempt: %s"), bAttachSuccess ? TEXT("True") : TEXT("False"));

		Piece->SetSnapped(true); // 상태 기록
		Piece->DestroyPickupComp(); // PickupComponent 제거

		PlayFeedback(true);

		CheckPuzzleCompleted();
	}
	else if (Dist > SnapRadius && GuideDist < GuideRadius)
	{
		// 튕겨나가는 효과
		if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(Piece->GetRootComponent()))
		{
			FVector PushDir = (Piece->GetActorLocation() - SnapPoint->GetComponentLocation()).GetSafeNormal();
			Prim->AddImpulse(PushDir * 150.f, NAME_None, true);
		}

		UE_LOG(LogTemp, Warning, TEXT("TrySnapPiece: Fail (Dist=%.1f)"), Dist);

		PlayFeedback(false);
	}
}

void ARestorePuzzleActor::CheckPuzzleCompleted()
{
	bool bAllSnapped = true;

	for (AActor* Piece : PieceActors)
	{
		if (APieceActor* P = Cast<APieceActor>(Piece))
		{
			if (!P->IsSnapped())
			{
				bAllSnapped = false;
				break;
			}
		}
	}

	if (bAllSnapped)
	{
		OnPuzzleCompleted();
	}
}

void ARestorePuzzleActor::OnPuzzleCompleted()
{
	UE_LOG(LogTemp, Log, TEXT("Puzzle Completed!"));

	// 모든 조각 제거
	TArray<AActor*> Pieces;
	SpawnedRelic->GetAllChildActors(Pieces);
	for (AActor* Piece : Pieces)
	{
		Piece->Destroy();
	}

	// 가이드 메시 제거 및 완성된 유물 메시 가시화
	/*UStaticMeshComponent* GuideMesh = Cast<UStaticMeshComponent>(SpawnedRelic->GetComponentsByTag(UStaticMeshComponent::StaticClass(), FName("Guide"))[0]);*/
	if (GuideMesh) GuideMesh->DestroyComponent(true);

	UStaticMeshComponent* CompletedMesh = Cast<UStaticMeshComponent>(SpawnedRelic->GetComponentsByTag(UStaticMeshComponent::StaticClass(), FName("Complete"))[0]);
	if (CompletedMesh)
	{
		CompletedMesh->SetHiddenInGame(false);
		CompletedMesh->SetCollisionProfileName(FName("GrabbingObject"));
	}
	
	// - 완료 이펙트/사운드 재생

	// 유물 복원 완료 처리
	RelicData.IsRecover = true;
	RestoreManager->NotifyPuzzleCompleted(this);
}

void ARestorePuzzleActor::PlayFeedback(bool bSuccess)
{
	if(bSuccess)
	{
		if (SuccessSFX) UGameplayStatics::PlaySoundAtLocation(this, SuccessSFX, GetActorLocation());
	}
	else
	{
		if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
		{
			if (FailSFX && FailHaptic)
			{
				UGameplayStatics::PlaySoundAtLocation(this, FailSFX, GetActorLocation());
				PC->PlayHapticEffect(FailHaptic, EControllerHand::Left);
				PC->PlayHapticEffect(FailHaptic, EControllerHand::Right);
			}
		}
	}
}

