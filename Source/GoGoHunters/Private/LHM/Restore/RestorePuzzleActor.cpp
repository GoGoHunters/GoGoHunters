// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/Restore/RestorePuzzleActor.h"
#include "LHM/Restore/PieceActor.h"
#include "LHM/Restore/RestoreManager.h"
#include "EngineUtils.h"

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
	UStaticMeshComponent* GuideMesh = Cast<UStaticMeshComponent>(SpawnedRelic->GetComponentsByTag(UStaticMeshComponent::StaticClass(), FName("Guide"))[0]);
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

	// SnapPoint 태그를 가진 씬컴포넌트를 찾아서 위치 저장
	TArray<USceneComponent*> SnapPoints;
	SpawnedRelic->GetRootComponent()->GetChildrenComponents(true, SnapPoints);

	SnapPoints.Sort([](const USceneComponent& A, const USceneComponent& B)
	{
		return A.GetName() < B.GetName();
	});

	SnapPointTransforms.Empty();
	for (USceneComponent* Scene : SnapPoints)
	{
		//USceneComponent* SnapPoint = Cast<USceneComponent>(Scene);
		if (Scene && Scene->ComponentHasTag("SnapPoints"))
		{
			Scene->SetWorldLocation(GuideMesh->GetComponentLocation());
			//UE_LOG(LogTemp, Warning, TEXT("Found SnapPoint: %s at %s"), *Scene->GetName(), *Scene->GetComponentLocation().ToString());
		}
		else if (Scene && Scene->ComponentHasTag("SnapPoint"))
		{
			SnapPointTransforms.Add(Scene->GetComponentTransform());
		}
	}

	/*for (int32 i = 0; i < SnapPointTransforms.Num(); ++i)
	{
		UE_LOG(LogTemp, Warning, TEXT("SnapPointTransforms(%d): %s"), i, *SnapPointTransforms[i].GetLocation().ToString());
	}*/

	// 조각 목록 저장
	PieceActors.Empty();
	TArray<AActor*> Pieces;
	SpawnedRelic->GetAllChildActors(Pieces);

	Pieces.Sort([](const AActor& A, const AActor& B)
	{
		return A.GetName() < B.GetName();
	});

	for (int32 i = 0; i < Pieces.Num(); ++i)
	{
		if (APieceActor* Piece = Cast<APieceActor>(Pieces[i]))
		{
			PieceActors.Add(Piece);
			Piece->SetPieceIndex(i); // 이름 순서대로 인덱스 지정
			//UE_LOG(LogTemp, Warning, TEXT("PieceActors(%d): %s"), i, *Piece->GetName());
		}
	}
#pragma endregion
}

void ARestorePuzzleActor::TrySnapPiece(class APieceActor* Piece)
{
	if (!Piece || !SnapPointTransforms.IsValidIndex(Piece->GetPieceIndex()))
		return;

	const int32 Index = Piece->GetPieceIndex();
	const FTransform& SnapT = SnapPointTransforms[Index];
	const float SnapRadius = 30.f;
	const float Dist = FVector::Dist(SnapT.GetLocation(), Piece->GetActorLocation());

	if (Dist < SnapRadius)
	{
		Piece->SetActorLocationAndRotation(SnapT.GetLocation(), SnapT.GetRotation().Rotator());
		if (UPrimitiveComponent* Primitive = Cast<UPrimitiveComponent>(Piece->GetRootComponent()))
		{
			Primitive->SetSimulatePhysics(false);
			Primitive->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
		Piece->SetSnapped(true); // 상태 기록
		CheckPuzzleCompleted();
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
	UStaticMeshComponent* GuideMesh = Cast<UStaticMeshComponent>(SpawnedRelic->GetComponentsByTag(UStaticMeshComponent::StaticClass(), FName("Guide"))[0]);
	if (GuideMesh) GuideMesh->DestroyComponent(true);

	UStaticMeshComponent* CompletedMesh = Cast<UStaticMeshComponent>(SpawnedRelic->GetComponentsByTag(UStaticMeshComponent::StaticClass(), FName("Complete"))[0]);
	if (CompletedMesh)
	{
		CompletedMesh->SetHiddenInGame(false);
		CompletedMesh->SetCollisionProfileName(FName("PhysicsActor"));
	}
	
	// - 완료 이펙트/사운드 재생

	// 유물 복원 완료 처리
	RelicData.IsRecover = true;
	RestoreManager->NotifyPuzzleCompleted(this);
}

