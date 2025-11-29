// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/Excavation/RelicsManager.h"
#include "LHM/Excavation/RelicsGround.h"
#include "Components/ChildActorComponent.h"
#include "LHM/Excavation/RelicsBase.h"
#include "LHM/Excavation/ExcavationManager.h"
#include "LHM/Excavation/CollectionBox.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Keyboard/AC_KeyBoard.h"
#include "LHM/Excavation/ExcavationProgressWidgetActor.h"
#include "LHM/Excavation/ExcavationWidgetActor.h"
#include "JMH/MH_VRPlayer.h"

// Sets default values
ARelicsManager::ARelicsManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	ExcavationLand_02 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ExcavationLand_02"));

	ExcavationLand_02->SetupAttachment(RootComponent);

	ExcavationLand_02->SetHiddenInGame(true);

	ExcavationLand_02->SetRelativeLocation(FVector(-27.5, 2.3, 154)); // (X=-27.577550,Y=2.345972,Z=154.206319)
	ExcavationLand_02->SetRelativeScale3D(FVector(0.66, 0.66, 0.45));

	ExcavationLand_02->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ExcavationLand_02->SetCollisionObjectType(ECC_WorldStatic);
	ExcavationLand_02->SetCollisionResponseToAllChannels(ECR_Block);

	ExcavationLand_02->bReceivesDecals = false;

	RelicsChild = CreateDefaultSubobject<UChildActorComponent>(TEXT("Relics"));
	RelicsChild->SetupAttachment(RootComponent);
	RelicsChild->SetChildActorClass(RelicsClass);
	RelicsChild->SetRelativeLocation(FVector(0.8, 2.3, 182)); // (X=-0.822835,Y=2.345971,Z=182.604621)

	static ConstructorHelpers::FClassFinder<ARelicsGround> GroundClassFinder(TEXT("/Game/LHM/BP/Excavation/BP_RelicsGround"));
	if (GroundClassFinder.Succeeded())
	{
		RelicsGroundClass = GroundClassFinder.Class;
		/*for (int32 i = 0; i < 3; ++i)
		{
			FString ChildName = FString::Printf(TEXT("GroundLayer_%d"), i + 1);
			if (UChildActorComponent* GroundChild = CreateDefaultSubobject<UChildActorComponent>(*ChildName))
			{
				GroundChild->SetupAttachment(RootComponent);
				GroundChild->SetChildActorClass(RelicsGroundClass);
				GroundChildActors.Add(GroundChild);
			}
		}*/

		GroundChild1 = CreateDefaultSubobject<UChildActorComponent>(TEXT("GroundLayer_1"));
		//GroundChild2 = CreateDefaultSubobject<UChildActorComponent>(TEXT("GroundLayer_2"));

		GroundChild1->SetupAttachment(RootComponent);
		//GroundChild2->SetupAttachment(RootComponent);

		GroundChild1->SetChildActorClass(RelicsGroundClass);
		//GroundChild2->SetChildActorClass(RelicsGroundClass);

		GroundChild1->SetRelativeLocation(FVector(-69, -13.6, 221)); // (X=-69.138980,Y=-13.695208,Z=221.159939)
		GroundChild1->SetRelativeScale3D(FVector(0.41));

		//GroundChild2->SetRelativeLocation(FVector(-69, -13.6, 201)); // (X=-69.138980,Y=-13.695208,Z=201.159939)
		//GroundChild2->SetRelativeScale3D(FVector(0.28));

		GroundChildActors.Add(GroundChild1);
		//GroundChildActors.Add(GroundChild2);
	}

	CurrentLayerIndex = 0;
	bBrushPhaseStarted = false;

}

// Called when the game starts or when spawned
void ARelicsManager::BeginPlay()
{
	Super::BeginPlay();

	GroundLayers.Empty();

	if (GroundChild1 /*&& GroundChild2*/)
	{
		Ground1 = Cast<ARelicsGround>(GroundChild1->GetChildActor());
		//Ground2 = Cast<ARelicsGround>(GroundChild2->GetChildActor());

		GroundLayers.Add(Ground1);
		//GroundLayers.Add(Ground2);

		Ground1->SetActorHiddenInGame(true); // 시작 시 숨김
		//Ground2->SetActorHiddenInGame(true); // 시작 시 숨김
		Ground1->SetRelicsManager(this);
		//Ground2->SetRelicsManager(this);
	}

	if (RelicsChild)
	{
		Relics = Cast<ARelicsBase>(RelicsChild->GetChildActor());
		Relics->SetRelicsManager(this);
	}

	if (ProgressClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;

		ProgressActor = GetWorld()->SpawnActor<AExcavationProgressWidgetActor>(ProgressClass, GetActorLocation(), GetActorRotation(), SpawnParams);
		if (ProgressActor)
		{
			ProgressActor->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
			ProgressActor->SetActorLocation(GetActorLocation() + FVector(-260, -60, 240)); // (X=-260.000000,Y=-60.000000,Z=240.000000)
			ProgressActor->SetActorHiddenInGame(true);
		}
	}

	if (PhaseUIActorClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;

		PhaseUIActor = GetWorld()->SpawnActor<AExcavationWidgetActor>(PhaseUIActorClass, GetActorLocation(), GetActorRotation(), SpawnParams);
		if (PhaseUIActor)
		{
			PhaseUIActor->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
			PhaseUIActor->SetActorLocation(GetActorLocation() + FVector(-200, 0, 340));
			PhaseUIActor->SetActorEnableCollision(false);
		}
	}
}

// Called every frame
void ARelicsManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARelicsManager::StartExcavation()
{
	if (GetRelics()) GetRelics()->ActivateMarker();

	for (auto Ground : GroundLayers)
	{
		if (IsValid(Ground))
		{
			Ground->SetActorHiddenInGame(false);
		}
	}

	// Excavation 지형 변경
	for (TActorIterator<AActor> It(GetWorld(), AActor::StaticClass()); It; ++It)
	{
		if (IsValid(*It) && (*It)->ActorHasTag(FName("LandPrime")))
		{
			It->Destroy();
			break;
		}
	}

	if (ExcavationLand_02) ExcavationLand_02->SetHiddenInGame(false);
	if (ProgressActor) ProgressActor->SetActorHiddenInGame(false);

	bBrushPhaseStarted = false;

	UE_LOG(LogTemp, Log, TEXT("[RelicsManager] Excavation started for: %s"), *GetName());
}

void ARelicsManager::NotifyGroundProgress(float Progress)
{
	if (bBrushPhaseStarted) return;

	if (CurrentLayerIndex >= GroundLayers.Num()) return;

	if (bPressedDevKey) Progress = 0.07f;

	PlayTamiCompliments(CurrentLayerIndex, Progress);

	if (Progress >= 0.07f) // 6% 이상 파괴되었으면
	{
		auto CurrentLayer = GroundLayers[CurrentLayerIndex];
		if (IsValid(CurrentLayer))
		{
			if (AMH_VRPlayer* Player = Cast<AMH_VRPlayer>(UGameplayStatics::GetPlayerCharacter(this, 0)))
			{
				Player->RelicsGroundRefs.Remove(CurrentLayer);
			}

			CurrentLayer->Destroy();
			//GroundLayers[CurrentLayerIndex] = nullptr;
			UE_LOG(LogTemp, Log, TEXT("Destroyed Ground Layer %d"), CurrentLayerIndex + 1);
		}

		//CurrentLayerIndex++;
		GroundLayers.RemoveAt(CurrentLayerIndex);
		UE_LOG(LogTemp, Log, TEXT("Destroyed Ground Layer %d / GroundLayers.num is %d"), CurrentLayerIndex + 1, GroundLayers.Num());

		//if (CurrentLayerIndex >= GroundLayers.Num())
		if (GroundLayers.Num() == 0)
		{
			bBrushPhaseStarted = true;
			EnterBrushPhase(); // 마지막 레이어 제거 후 붓 단계로 전환
			return;
		}
	}
}

void ARelicsManager::EnterBrushPhase()
{
	for (TActorIterator<AExcavationManager> It(GetWorld()); It; ++It)
	{
		It->NotifyExcavationCompleted(this);
		break;
	}
}

void ARelicsManager::SpawnCollectionBox()
{
	if (!IsValid(Relics)) return;
	if (IsValid(CollectionBox)) return;
	if (!CollectionBoxClass) return;

	FVector SpawnLocation = GetActorLocation() + FVector(0.5, -72, 192); // (X=0.500000,Y=-72.000000,Z=192.000000)
	FRotator SpawnRotation = GetActorRotation() + FRotator(0, 115, 0);

	FActorSpawnParameters Params;
	Params.Owner = this;

	CollectionBox = GetWorld()->SpawnActor<ACollectionBox>(CollectionBoxClass, SpawnLocation, SpawnRotation, Params);

	if (CollectionBox)
	{
		CollectionBox->SetTargetRelic(Relics);
		CollectionBox->SetRelicsManager(this);
		CollectionBox->SetInitialRelicTransforms(Relics);
		UE_LOG(LogTemp, Log, TEXT("[RelicsManager] CollectionBox 스폰 완료"));
	}
}

bool ARelicsManager::GetCurrentDigProgress(float& OutProgress) const
{
	int32 TotalLayers = GroundLayers.Num();

	/*if (TotalLayers == 0)
	{
		OutProgress = 0.0f;
		return false;
	}*/

	// 모든 레이어가 제거된 경우 100%로 간주
	if (TotalLayers == 0)
	{
		OutProgress = 1.0f;
		return true;
	}

	float TotalProgress = 0.0f;

	for (int32 i = 0; i < TotalLayers; ++i)
	{
		// 배열 인덱스 안전 검사
		if (!GroundLayers.IsValidIndex(i))
		{
			// 존재하지 않는 레이어 → 이미 제거된 것으로 간주
			TotalProgress += 1.0f;
			continue;
		}

		ARelicsGround* Ground = GroundLayers[i];
		if (!IsValid(Ground)) // Destroy된 레이어는 파괴 완료로 간주
		{
			TotalProgress += 1.0f;
			continue;
		}

		// RenderTarget 안전 검사
		if (!IsValid(Ground->HeightFieldRT) || !Ground->HeightFieldRT->GetResource())
		{
			UE_LOG(LogTemp, Warning, TEXT("[RelicsManager] Layer %d HeightFieldRT invalid → treated as destroyed"), i);
			TotalProgress += 1.0f; // 이미 파괴된 걸로 간주
			continue;
		}

		// 정상적인 파괴도 계산
		float Destruction = Ground->CalculateDestructionFromRenderTarget();
		float Normalized = FMath::Clamp(Destruction / 0.07f, 0.0f, 1.0f); // layer[0]: 6%/layer[i]: 1% 기준으로 정규화
		TotalProgress += Normalized;
	}

	// 평균 계산 (0~1)
	OutProgress = TotalProgress / static_cast<float>(TotalLayers); // 전체 평균
	return true;
}

void ARelicsManager::SpawnKeyboard()
{
	if (!IsValid(Relics)) return;
	if (!KeyBoardClass) return;
	if (KeyBoardActor) return;

	FVector SpawnLocation = GetActorLocation() + FVector(-169, 20, 290);
	FRotator SpawnRotation = GetActorRotation() + FRotator(0, 270, 70); // (Pitch=0.000000,Yaw=90.000000,Roll=-70.000000)

	FActorSpawnParameters Params;
	Params.Owner = this;

	KeyBoardActor = GetWorld()->SpawnActor<AAC_KeyBoard>(KeyBoardClass, SpawnLocation, SpawnRotation, Params);
	UE_LOG(LogTemp, Log, TEXT("[RelicsManager] Keyboard 스폰 완료"));
}

void ARelicsManager::DestroyKeyboard()
{
	if (KeyBoardActor) KeyBoardActor->Destroy();
}

void ARelicsManager::PlayTamiCompliments(int32 CurrentLayer, float Progress)
{
	// 레이어와 진행도에 따른 1회성 재생 가드
	if (CurrentLayerIndex == 0)
	{
		if (!bPlayedCompliment2 && Progress >= 0.02f)
		{
			bPlayedCompliment2 = true;
			PlayTami(TEXT("PlayExcavationCompliment2"));
		}
	}
	else if (CurrentLayerIndex == 1)
	{
		if (!bPlayedVisibleRelic && Progress >= 0.01f)
		{
			bPlayedVisibleRelic = true;
			PlayTami(TEXT("PlayExcavationPhase3_VisibleRelic"));
		}

		if (!bPlayedCompliment3 && Progress >= 0.04f)
		{
			bPlayedCompliment3 = true;
			PlayTami(TEXT("PlayExcavationCompliment3"));
		}
	}
}

void ARelicsManager::PlayTami(const FName& FunctionName)
{
	for (TActorIterator<APawn> It(GetWorld(), APawn::StaticClass()); It; ++It)
	{
		if (IsValid(*It) && (*It)->ActorHasTag(FName("Tami")))
		{
			if (APawn* TamiAI = *It)
			{
				if (UFunction* Function = TamiAI->FindFunction(FunctionName))
				{
					TamiAI->ProcessEvent(Function, nullptr);
				}
			}
			break;
		}
	}
}

