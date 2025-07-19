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

void AExcavationManager::NotifyDetectionCompleted(class ARelicsManager* FromManager)
{
	if (!IsValid(FromManager)) return;

	CurrentActiveManager = FromManager;
	FromManager->StartExcavation();

	UE_LOG(LogTemp, Log, TEXT("[ExcavationManager] 유물 발견! 땅 파기 단계로 전환: %s"), *FromManager->GetName());

	// TODO: 삽 도구 전환, UI 표시 등
	
}

void AExcavationManager::NotifyExcavationCompleted(class ARelicsManager* FromManager)
{
	if (!IsValid(FromManager)) return;

	UE_LOG(LogTemp, Log, TEXT("[ExcavationManager] 땅 파기 완료! 붓질 단계로 전환: %s"), *FromManager->GetName());
	
	// TODO: 붓 도구 전환, UI 표시 등

}

void AExcavationManager::NotifyDustingCompleted(class ARelicsManager* FromManager)
{
	if (!IsValid(FromManager)) return;

	UE_LOG(LogTemp, Log, TEXT("[ExcavationManager] 붓질 완료! 수거 단계로 전환: %s"), *FromManager->GetName());

	FromManager->SpawnCollectionBox(); // 수거박스 생성 요청

	// TODO: 수거 UI 출력, 수거 도구 활성화 등
}

void AExcavationManager::NotifyCollectionCompleted(class ARelicsManager* FromManager)
{
	if (!IsValid(FromManager)) return;

	UE_LOG(LogTemp, Log, TEXT("[ExcavationManager] 모든 유물 수거 완료! 복원실 전송 준비: %s"), *FromManager->GetName());

	// TODO: 복원실 전송, 트리거 활성화 등
}

