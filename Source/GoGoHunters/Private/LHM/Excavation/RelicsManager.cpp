// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/Excavation/RelicsManager.h"
#include "LHM/Excavation/RelicsGround.h"
#include "Components/ChildActorComponent.h"
#include "LHM/Excavation/RelicsBase.h"

// Sets default values
ARelicsManager::ARelicsManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	ExcavationLand_01 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ExcavationLand_01"));
	ExcavationLand_02 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ExcavationLand_02"));
	ExcavationSite = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ExcavationSite"));

	ExcavationLand_01->SetupAttachment(RootComponent);
	ExcavationLand_02->SetupAttachment(RootComponent);
	ExcavationSite->SetupAttachment(RootComponent);

	ExcavationLand_01->SetHiddenInGame(false);
	ExcavationLand_02->SetHiddenInGame(true);
	ExcavationSite->SetHiddenInGame(true);

	ExcavationLand_02->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	ExcavationSite->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);

	RelicsChild = CreateDefaultSubobject<UChildActorComponent>(TEXT("Relics"));
	RelicsChild->SetupAttachment(RootComponent);
	RelicsChild->SetChildActorClass(RelicsClass);

	for (int32 i = 0; i < 3; ++i)
	{
		FString ChildName = FString::Printf(TEXT("GroundLayer_%d"), i + 1);
		UChildActorComponent* GroundChild = CreateDefaultSubobject<UChildActorComponent>(*ChildName);
		GroundChild->SetupAttachment(RootComponent);
		GroundChildActors.Add(GroundChild);
	}

	CurrentLayerIndex = 0;
	bBrushPhaseStarted = false;

}

// Called when the game starts or when spawned
void ARelicsManager::BeginPlay()
{
	Super::BeginPlay();
	
	GroundLayers.Empty();

	for (auto Child: GroundChildActors)
	{
		if (auto* Ground = Cast<ARelicsGround>(Child->GetChildActor()))
		{
			GroundLayers.Add(Ground);
			Ground->SetActorHiddenInGame(true); // 시작 시 숨김
			Ground->SetRelicsManager(this);
		}
	}

	if (RelicsChild)
	{
		Relics = Cast<ARelicsBase>(RelicsChild->GetChildActor());
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
	if (ExcavationLand_01)
		ExcavationLand_01->DestroyComponent();

	if (ExcavationLand_02)
		ExcavationLand_02->SetHiddenInGame(false);

	if (ExcavationSite)
		ExcavationSite->SetHiddenInGame(false);

	bBrushPhaseStarted = false;

	UE_LOG(LogTemp, Log, TEXT("[RelicsManager] Excavation started for: %s"), *GetName());
}

void ARelicsManager::NotifyGroundProgress(float Progress)
{
	if (CurrentLayerIndex >= GroundLayers.Num()) return;

	if (Progress >= 0.5f) // 50% 이상 파괴되었으면
	{
		auto CurrentLayer = GroundLayers[CurrentLayerIndex];
		if (IsValid(CurrentLayer))
		{
			CurrentLayer->Destroy();
			UE_LOG(LogTemp, Log, TEXT("Destroyed Ground Layer %d"), CurrentLayerIndex + 1);
		}

		CurrentLayerIndex++;

		if (CurrentLayerIndex >= GroundLayers.Num())
		{
			EnterBrushPhase(); // 마지막 레이어 제거 후 붓 단계로 전환
		}
	}
}

void ARelicsManager::EnterBrushPhase()
{
	UE_LOG(LogTemp, Log, TEXT("EnterBrushPhase"));
}

