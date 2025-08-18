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

// Sets default values
ARelicsManager::ARelicsManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	//ExcavationLand_01 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ExcavationLand_01"));
	ExcavationLand_02 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ExcavationLand_02"));

	//ExcavationLand_01->SetupAttachment(RootComponent);
	ExcavationLand_02->SetupAttachment(RootComponent);

	//ExcavationLand_01->SetHiddenInGame(false);
	ExcavationLand_02->SetHiddenInGame(true);

	//ExcavationLand_01->SetRelativeLocation(FVector(0,0, 80)); // (X=0.000000,Y=0.000000,Z=80.000000)
	ExcavationLand_02->SetRelativeLocation(FVector(-27.5, 2.3, 154)); // (X=-27.577550,Y=2.345972,Z=154.206319)
	ExcavationLand_02->SetRelativeScale3D(FVector(0.66, 0.66, 0.45));

	ExcavationLand_02->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ExcavationLand_02->SetCollisionObjectType(ECC_WorldStatic);
	ExcavationLand_02->SetCollisionResponseToAllChannels(ECR_Block);
	
	//ExcavationLand_01->bReceivesDecals = false;
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
		GroundChild2 = CreateDefaultSubobject<UChildActorComponent>(TEXT("GroundLayer_2"));
		//GroundChild3 = CreateDefaultSubobject<UChildActorComponent>(TEXT("GroundLayer_3"));
		GroundChild1->SetupAttachment(RootComponent);
		GroundChild2->SetupAttachment(RootComponent);
		//GroundChild3->SetupAttachment(RootComponent);
		GroundChild1->SetChildActorClass(RelicsGroundClass);
		GroundChild2->SetChildActorClass(RelicsGroundClass);
		//GroundChild3->SetChildActorClass(RelicsGroundClass);

		GroundChild1->SetRelativeLocation(FVector(-69, -13.6, 221)); // (X=-69.138980,Y=-13.695208,Z=221.159939)
		GroundChild1->SetRelativeScale3D(FVector(0.41));

		GroundChild2->SetRelativeLocation(FVector(-69, -13.6, 201)); // (X=-69.138980,Y=-13.695208,Z=201.159939)
		GroundChild2->SetRelativeScale3D(FVector(0.28));

		GroundChildActors.Add(GroundChild1);
		GroundChildActors.Add(GroundChild2);
		//GroundChildActors.Add(GroundChild3);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("RelicsGroundClass not found!"));
	}

	CurrentLayerIndex = 0;
	bBrushPhaseStarted = false;

}

// Called when the game starts or when spawned
void ARelicsManager::BeginPlay()
{
	Super::BeginPlay();
	
	//GroundLayers.Empty();
	//for (auto Child: GroundChildActors)
	//{
	//	if (Child)
	//	{
	//		if (Child->GetChildActor())
	//		{
	//			if (auto* Ground = Cast<ARelicsGround>(Child->GetChildActor()))
	//			{
	//				GroundLayers.Add(Ground);
	//				Ground->SetActorHiddenInGame(true); // 시작 시 숨김
	//				Ground->SetRelicsManager(this);
	//				UE_LOG(LogTemp, Log, TEXT("[RelicsManager] Ground Layer %d: %s"), GroundLayers.Num(), *Ground->GetName());
	//				UE_LOG(LogTemp, Log, TEXT("[RelicsManager] Child Actor Class: %s"), *Child->GetChildActorClass()->GetName());
	//				UE_LOG(LogTemp, Log, TEXT("[RelicsManager] Child Actor Name: %s"), *Child->GetChildActor()->GetName());
	//			}
	//		}
	//	}
	//	else
	//	{
	//		UE_LOG(LogTemp, Log, TEXT("Child is null..."));
	//	}
	//}

	GroundLayers.Empty();

	if (GroundChild1 /*&& GroundChild2 && GroundChild3*/)
	{
		Ground1 = Cast<ARelicsGround>(GroundChild1->GetChildActor());
		Ground2 = Cast<ARelicsGround>(GroundChild2->GetChildActor());
		//Ground3 = Cast<ARelicsGround>(GroundChild3->GetChildActor());

		GroundLayers.Add(Ground1);
		GroundLayers.Add(Ground2);
		//GroundLayers.Add(Ground3);

		Ground1->SetActorHiddenInGame(true); // 시작 시 숨김
		Ground2->SetActorHiddenInGame(true); // 시작 시 숨김
		//Ground3->SetActorHiddenInGame(true); // 시작 시 숨김
		Ground1->SetRelicsManager(this);
		Ground2->SetRelicsManager(this);
		//Ground3->SetRelicsManager(this);
	}

	if (RelicsChild)
	{
		Relics = Cast<ARelicsBase>(RelicsChild->GetChildActor());
		Relics->SetRelicsManager(this);
	}
}

// Called every frame
void ARelicsManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARelicsManager::StartExcavation()
{
	for (auto Ground : GroundLayers)
	{
		if (IsValid(Ground))
		{
			Ground->SetActorHiddenInGame(false);
		}
	}

	// Excavation 지형 변경
	//if (ExcavationLand_01)
	//	ExcavationLand_01->DestroyComponent();
	for (TActorIterator<AActor> It(GetWorld(), AActor::StaticClass()); It; ++It)
	{
		if (IsValid(*It) && (*It)->ActorHasTag(FName("LandPrime")))
		{
			It->Destroy();
			break;
		}
	}

	if (ExcavationLand_02)
		ExcavationLand_02->SetHiddenInGame(false);

	bBrushPhaseStarted = false;

	UE_LOG(LogTemp, Log, TEXT("[RelicsManager] Excavation started for: %s"), *GetName());
}

void ARelicsManager::NotifyGroundProgress(float Progress)
{
	if (CurrentLayerIndex >= GroundLayers.Num()) return;

	if (bPressedDevKey) Progress = 0.10f;

	if (Progress >= 0.10f) // 15% 이상 파괴되었으면
	{
		auto CurrentLayer = GroundLayers[CurrentLayerIndex];
		if (IsValid(CurrentLayer))
		{
			CurrentLayer->Destroy();
			GroundLayers[CurrentLayerIndex] = nullptr;
			UE_LOG(LogTemp, Log, TEXT("Destroyed Ground Layer %d"), CurrentLayerIndex + 1);
		}

		CurrentLayerIndex++;

		// 타미 음성
		if (CurrentLayerIndex == 1 && Progress >= 0.03f)
		{
			for (TActorIterator<APawn> It(GetWorld(), APawn::StaticClass()); It; ++It)
			{
				if (IsValid(*It) && (*It)->ActorHasTag(FName("Tami")))
				{
					if (APawn* TamiAI = *It)
					{
						FName FunctionName(TEXT("PlayExcavationPhase3_VisibleRelic"));
						if (UFunction* Function = TamiAI->FindFunction(FunctionName))
						{
							TamiAI->ProcessEvent(Function, nullptr);
						}
					}
					break;
				}
			}
		}

		UE_LOG(LogTemp, Log, TEXT("Destroyed Ground Layer %d / GroundLayers.num is %d"), CurrentLayerIndex + 1, GroundLayers.Num());

		if (CurrentLayerIndex >= GroundLayers.Num())
		{
			EnterBrushPhase(); // 마지막 레이어 제거 후 붓 단계로 전환
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

	FVector SpawnLocation = GetActorLocation() + FVector(25, -46, 191); // (X=25.000000,Y=-46.000000,Z=191.000000)
	FRotator SpawnRotation = GetActorRotation() + FRotator(0, 125, 0);

	FActorSpawnParameters Params;
	Params.Owner = this;

	CollectionBox = GetWorld()->SpawnActor<ACollectionBox>(CollectionBoxClass, SpawnLocation, SpawnRotation, Params);

	if (CollectionBox)
	{
		CollectionBox->SetTargetRelic(Relics);
		CollectionBox->SetRelicsManager(this);
		UE_LOG(LogTemp, Log, TEXT("[RelicsManager] CollectionBox 스폰 완료"));
	}
}

bool ARelicsManager::GetCurrentDigProgress(float& OutProgress) const
{
	int32 TotalLayers = GroundLayers.Num();
	
	if (TotalLayers == 0)
	{
		OutProgress = 0.0f;
		return false;
	}
	
	float TotalProgress = 0.0f;

	for(int32 i = 0; i < TotalLayers; ++i)
	{
		ARelicsGround* Ground = GroundLayers[i];
		if(!IsValid(Ground)) // Destroy된 레이어는 파괴 완료로 간주
		{
			TotalProgress += 1.0f;
		}
		else if (!IsValid(Ground->HeightFieldRT) || !Ground->HeightFieldRT->GetResource())
		{
			UE_LOG(LogTemp, Warning, TEXT("Layer %d HeightFieldRT is invalid after destroy"), i);
			TotalProgress += 1.0f; // 이미 파괴된 걸로 간주
		}
		else
		{
			float Destruction = Ground->CalculateDestructionFromRenderTarget();
			float Normalized = FMath::Clamp(Destruction / 0.15f, 0.0f, 1.0f); // 15% 기준으로 정규화
			TotalProgress += Normalized;
		}
	}

	OutProgress = TotalProgress / static_cast<float>(TotalLayers); // 전체 평균
	return true;
}

