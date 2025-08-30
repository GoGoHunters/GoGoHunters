// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/Restore/RestoreManager.h"
#include "LHM/UI/RestoreUI.h"
#include "base/GI_Base.h"
#include "LHM/Restore/RestorePuzzleActor.h"
#include "LHM/Restore/RestorWidgetActor.h"
#include "EngineUtils.h"

// Sets default values
ARestoreManager::ARestoreManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ARestoreManager::BeginPlay()
{
	Super::BeginPlay();

}

void ARestoreManager::StartRestoration(const FCRelicData& RelicData)
{
	CurrentRelicData = RelicData;

	//if (RestoreUI) RestoreUI->RemoveFromParent(); // UI 숨기기

	SpawnPuzzleActor(RelicData);
}

void ARestoreManager::SpawnPuzzleActor(const FCRelicData& RelicData)
{
	if (PuzzleActorClass)
	{
		FActorSpawnParameters SpawnParams;
		ActivePuzzleActor = GetWorld()->SpawnActor<ARestorePuzzleActor>(PuzzleActorClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

		if (ActivePuzzleActor)
		{
			ActivePuzzleActor->InitPuzzle(RelicData);
		}
	}
}

