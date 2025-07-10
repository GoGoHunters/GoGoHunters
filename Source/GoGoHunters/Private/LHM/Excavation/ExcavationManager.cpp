// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/Excavation/ExcavationManager.h"
#include "LHM/Excavation/RelicsManager.h"
#include "EngineUtils.h"

// Sets default values
AExcavationManager::AExcavationManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AExcavationManager::BeginPlay()
{
	Super::BeginPlay();
	
	for (TActorIterator<ARelicsManager> It(GetWorld()); It; ++It)
	{
		AllRelicsManagers.Add(*It);
	}
}

// Called every frame
void AExcavationManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AExcavationManager::StartRelicsExcavation(class ARelicsManager* Target)
{
	if (!IsValid(Target)) return;

	CurrentActiveManager = Target;

	UE_LOG(LogTemp, Log, TEXT("[ExcavationManager] Starting excavation for RelicsManager: %s"), *Target->GetName());

	Target->StartExcavation();
}

void AExcavationManager::OnBrushPhaseEntered(class ARelicsManager* Target)
{
	if (!IsValid(Target)) return;

	if (Target == CurrentActiveManager)
	{
		// TODO: 붓 도구 전환, UI 표시, 플레이어 상태 업데이트 등
		UE_LOG(LogTemp, Log, TEXT("[ExcavationManager] Brush phase entered for: %s"), *Target->GetName());
	}
}

