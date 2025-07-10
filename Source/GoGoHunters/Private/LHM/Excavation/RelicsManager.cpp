// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/Excavation/RelicsManager.h"
#include "LHM/Excavation/RelicsGround.h"

// Sets default values
ARelicsManager::ARelicsManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ARelicsManager::BeginPlay()
{
	Super::BeginPlay();
	
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
		ExcavationLand_01->SetHiddenInGame(true);

	if (ExcavationLand_02)
		ExcavationLand_02->SetHiddenInGame(false);

	if (ExcavationSite)
		ExcavationSite->SetHiddenInGame(false);

	CurrentLayerIndex = 0;
	bBrushPhaseStarted = false;

	UE_LOG(LogTemp, Log, TEXT("[RelicsManager] Excavation started for: %s"), *GetName());
}

void ARelicsManager::NotifyGroundProgress(float Progress)
{
	if (CurrentLayerIndex >= GroundLayers.Num()) return;

	if (Progress >= 0.85f) // 85% 이상 파괴되었으면
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

}

