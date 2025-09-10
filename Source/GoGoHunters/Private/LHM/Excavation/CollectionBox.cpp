// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/Excavation/CollectionBox.h"
#include "Components/BoxComponent.h"
#include "LHM/Excavation/RelicsBase.h"
#include "Kismet/GameplayStatics.h"
#include "LHM/Excavation/ExcavationManager.h"
#include "EngineUtils.h"
#include "LHM/UI/BrushingUI.h"
#include "LHM/UI/DecalProgressUI.h"
#include "Components/WidgetComponent.h"
#include "LHM/UI/CollectionBoxUI.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ACollectionBox::ACollectionBox()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));

	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
	TriggerVolume->SetupAttachment(RootComponent);
	TriggerVolume->SetBoxExtent(FVector(20));
	TriggerVolume->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	TriggerVolume->SetGenerateOverlapEvents(true);

	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClassFinder(TEXT("/Game/LHM/UI/WBP_CloseBtnUI"));
	if (WidgetClassFinder.Succeeded())
	{
		WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("CloseBtnUI"));
		WidgetComponent->SetWidgetClass(WidgetClassFinder.Class);
		WidgetComponent->SetupAttachment(RootComponent);
		WidgetComponent->SetWidgetSpace(EWidgetSpace::World);
		WidgetComponent->SetDrawSize(FVector2D(200, 100));
		WidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

// Called when the game starts or when spawned
void ACollectionBox::BeginPlay()
{
	Super::BeginPlay();

	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &ACollectionBox::OnOverlapBegin);
	TriggerVolume->OnComponentEndOverlap.AddDynamic(this, &ACollectionBox::OnOverlapEnd);

	if (WidgetComponent)
	{
		UUserWidget* UserWidget = WidgetComponent->GetWidget();
		if (UCollectionBoxUI* CloseBtnUI = Cast<UCollectionBoxUI>(UserWidget))
		{
			CloseBtnUI->SetOwningWidgetActor(this);
		}
	}
}

// Called every frame
void ACollectionBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bVisibleCloseBtnUI)
	{
		if (WidgetComponent)
		{
			APlayerCameraManager* CamManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
			if (CamManager)
			{
				FVector CamLocation = CamManager->GetCameraLocation();
				FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(WidgetComponent->GetComponentLocation(), CamLocation);
				WidgetComponent->SetWorldRotation(LookAtRotation);
			}
		}
	}
}

void ACollectionBox::OnOverlapBegin(UPrimitiveComponent* Overlapped, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	ARelicsBase* Relic = Cast<ARelicsBase>(OtherActor);
	if (!Relic) return;

	for (UStaticMeshComponent* Mesh : Relic->RelicsMeshes)
	{
		if (OtherComp == Mesh)
		{
			// 수거 처리: Detach + 물리/충돌 제거
			Mesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
			Mesh->ComponentTags.Add(FName("Collected"));

			// 이펙트/사운드/텍스트
			UGameplayStatics::PlaySound2D(GetWorld(), CollectionSFX); // 수거 효과음
			CollectedMeshes.Add(Mesh);

			UE_LOG(LogTemp, Log, TEXT("[CollectionBox] 유물 수거됨: %s"), *Relic->GetName());

			// UI 업데이트
			/*if (Relic->GetBrushingUI() && Relic->GetBrushingUI()->GetMeshToWidgetMap().Contains(Mesh))
			{
				if (UDecalProgressUI* Widget = Relic->GetBrushingUI()->GetMeshToWidgetMap()[Mesh])
				{
					Widget->SetCollectedImage(true);
				}
			}*/
			if (Relic->GetBrushingUI())
			{
				Relic->GetBrushingUI()->SetCollectedImage(true);
			}

			CheckAllCollected();
			break;
		}
	}
}

void ACollectionBox::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ARelicsBase* Relic = Cast<ARelicsBase>(OtherActor);
	if (!Relic) return;

	for (UStaticMeshComponent* Mesh : Relic->RelicsMeshes)
	{
		if (OtherComp == Mesh)
		{
			// 수거 목록에서 제거
			Mesh->ComponentTags.Remove(FName("Collected"));
			CollectedMeshes.Remove(Mesh);

			// UI 되돌리기
			/*if (Relic->GetBrushingUI() && Relic->GetBrushingUI()->GetMeshToWidgetMap().Contains(Mesh))
			{
				if (UDecalProgressUI* Widget = Relic->GetBrushingUI()->GetMeshToWidgetMap()[Mesh])
				{
					Widget->SetCollectedImage(false);
				}
			}*/
			if (Relic->GetBrushingUI())
			{
				Relic->GetBrushingUI()->SetCollectedImage(false);
			}

			break;
		}
	}
}

void ACollectionBox::SetInitialRelicTransforms(class ARelicsBase* Relic)
{
	// 초기 위치 저장
	if (Relic)
	{
		for (UStaticMeshComponent* Mesh : Relic->RelicsMeshes)
		{
			if (Mesh)
			{
				InitialRelicTransforms.Add(Mesh, Mesh->GetComponentTransform());
			}
		}
	}
}

void ACollectionBox::CheckAllCollected()
{
	if(!TargetRelic) return;

	const int32 TotalCount = TargetRelic->RelicsMeshes.Num();
	const int32 CollectedCount = CollectedMeshes.Num();

	UE_LOG(LogTemp, Log, TEXT("[CollectionBox] Relics collected (%d/%d)"), CollectedCount, TotalCount);

	if (CollectedCount == 1)
	{
		for (TActorIterator<APawn> It(GetWorld(), APawn::StaticClass()); It; ++It)
		{
			if (IsValid(*It) && (*It)->ActorHasTag(FName("Tami")))
			{
				if (APawn* TamiAI = *It)
				{
					if (UFunction* Function = TamiAI->FindFunction(TEXT("PlayExcavationCompliment4")))
					{
						TamiAI->ProcessEvent(Function, nullptr);
					}
				}
				break;
			}
		}
	}

	if (TotalCount > 0 && CollectedCount == TotalCount)
	{
		UE_LOG(LogTemp, Log, TEXT("[CollectionBox] 모든 유물 총 %d개 수거 완료!"), CollectedCount);
		//PlayBoxCloseAnimation();

		for (TActorIterator<AExcavationManager> It(GetWorld()); It; ++It)
		{
			It->NotifyCollectionCompleted(RelicsManager, this);
			break;
		}
	}
}

void ACollectionBox::ResetCollectedRelics()
{
	// [1] 모든 유물 메시들의 트랜스폼 복구 및 태그 제거
	if (TargetRelic)
	{
		for (UStaticMeshComponent* Mesh : TargetRelic->RelicsMeshes)
		{
			if (!Mesh) continue;

			// 초기 위치 복구
			if (InitialRelicTransforms.Contains(Mesh))
			{
				Mesh->SetWorldTransform(InitialRelicTransforms[Mesh]);
			}

			// 태그 복구 (Collected 제거)
			Mesh->ComponentTags.Remove(FName("Collected"));

			// 물리/충돌 복구
			Mesh->SetSimulatePhysics(false);
			Mesh->SetCollisionProfileName(FName("Relic_Buried"));
			Mesh->SetGenerateOverlapEvents(true);
		}
	}

	// [2] 수거 배열 초기화
	CollectedMeshes.Empty();

	// [3] UI 초기화
	if (TargetRelic && TargetRelic->GetBrushingUI())
	{
		for (int32 i = 0; i < TargetRelic->RelicsMeshes.Num(); ++i)
		{
			TargetRelic->GetBrushingUI()->SetCollectedImage(false);
		}
	}
}

void ACollectionBox::PlayBoxCloseAnimation()
{
	K2_CloseLid();
}

void ACollectionBox::SetColleionCloseBtnUI(bool bVisible)
{
	if (bVisible)
	{
		bVisibleCloseBtnUI = true;
		WidgetComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		WidgetComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		WidgetComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
		WidgetComponent->SetCollisionProfileName("VRUI");
	}
	else
	{
		bVisibleCloseBtnUI = false;
		WidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void ACollectionBox::PressedDevKey()
{
	/*for (UStaticMeshComponent* Mesh : TargetRelic->RelicsMeshes)
	{
		if (Mesh->ComponentTags.Contains("Collected")) Mesh->SetCollisionProfileName(FName("Relic_Buried"));
	}*/
	
	for (UStaticMeshComponent* Mesh : TargetRelic->RelicsMeshes)
	{
		if (Mesh->ComponentTags.Contains("Collected")) continue;
		else Mesh->ComponentTags.Add(FName("Collected"));

		Mesh->SetSimulatePhysics(true);
		Mesh->SetCollisionProfileName(FName("Relic_Physics"));
		Mesh->SetCollisionResponseToChannel( ECollisionChannel::ECC_GameTraceChannel10, ECollisionResponse::ECR_Ignore);
		Mesh->SetGenerateOverlapEvents(true);
		Mesh->BodyInstance.bUseCCD = true; // 빠르게 낙하 시 충돌 누락 방지

		FVector Loc = GetActorLocation();
		FRotator Rot = GetActorRotation();

		// 위치 조정
		if (Mesh->GetName().Contains("RelicMesh_1"))
		{
			Loc += FVector(-0.5, 20, 55);
			Rot += FRotator(-60, -90, 90);
		}
		else if (Mesh->GetName().Contains("RelicMesh_2"))
		{
			Loc += FVector(21, 11, 10);
			Rot += FRotator(-63, 0, 0);
		}
		else if (Mesh->GetName().Contains("RelicMesh_3"))
		{
			Loc += FVector(-1, 5, 12);
			Rot += FRotator(-75, 137, 223);
		}
		else if (Mesh->GetName().Contains("RelicMesh_4"))
		{
			Loc += FVector(-18, -20, 10);
			Rot += FRotator(-84, 130, 118);
		}
		else if (Mesh->GetName().Contains("RelicMesh_5"))
		{
			Loc += FVector(15.5, -23, 11);
			Rot += FRotator(-72, -20, 20);
		}
		else if (Mesh->GetName().Contains("RelicMesh_6"))
		{
			Loc += FVector(18, -21, 14.5);
			Rot += FRotator(-61, -41, 37);
		}
		else if (Mesh->GetName().Contains("RelicMesh_7"))
		{
			Loc += FVector(21, -5.7, 8);
			Rot += FRotator(-87, 0, 0);
		}
		else if (Mesh->GetName().Contains("RelicMesh_8"))
		{
			Loc += FVector(2, -5.6, 13);
			Rot += FRotator(67, 111, 110);
		}

		Mesh->SetWorldLocation(Loc);
		Mesh->SetWorldRotation(Rot);

		CollectedMeshes.Add(Mesh);

		if (TargetRelic->GetBrushingUI())
		{
			TargetRelic->GetBrushingUI()->SetAllCollected();
		}
	}

	CheckAllCollected();
}

