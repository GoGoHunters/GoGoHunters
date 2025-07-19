// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/Excavation/ExcavationManager.h"
#include "LHM/Excavation/RelicsManager.h"
#include "EngineUtils.h"

// Sets default values
AExcavationManager::AExcavationManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 기본 단계별 도구 가용성 설정
	ToolAvailabilityByPhase.SetNum(4); // 4개 도구
	ToolAvailabilityByPhase[0] = true;  // 탐지 도구 - 항상 사용 가능
	ToolAvailabilityByPhase[1] = false; // 삽 도구 - 삽질 단계에서만
	ToolAvailabilityByPhase[2] = false; // 붓 도구 - 붓질 단계에서만
	ToolAvailabilityByPhase[3] = false; // 집게 도구 - 수거 단계에서만
}

// Called when the game starts or when spawned
void AExcavationManager::BeginPlay()
{
	Super::BeginPlay();
	
	for (TActorIterator<ARelicsManager> It(GetWorld()); It; ++It)
	{
		AllRelicsManagers.Add(*It);
	}

	// 초기 단계 설정
	SetCurrentPhase(EExcavationPhase::Detection);
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

	// 삽질 단계로 전환
	SetCurrentPhase(EExcavationPhase::Digging);
}

void AExcavationManager::NotifyExcavationCompleted(class ARelicsManager* FromManager)
{
	if (!IsValid(FromManager)) return;

	UE_LOG(LogTemp, Log, TEXT("[ExcavationManager] 땅 파기 완료! 붓질 단계로 전환: %s"), *FromManager->GetName());
	
	// 붓질 단계로 전환
	SetCurrentPhase(EExcavationPhase::Brushing);
}

void AExcavationManager::NotifyDustingCompleted(class ARelicsManager* FromManager)
{
	if (!IsValid(FromManager)) return;

	UE_LOG(LogTemp, Log, TEXT("[ExcavationManager] 붓질 완료! 수거 단계로 전환: %s"), *FromManager->GetName());

	FromManager->SpawnCollectionBox(); // 수거박스 생성 요청

	// 수거 단계로 전환
	SetCurrentPhase(EExcavationPhase::Collection);
}

void AExcavationManager::NotifyCollectionCompleted(class ARelicsManager* FromManager)
{
	if (!IsValid(FromManager)) return;

	UE_LOG(LogTemp, Log, TEXT("[ExcavationManager] 수거 완료! 발굴 완료: %s"), *FromManager->GetName());

	// 발굴 완료 단계로 전환
	SetCurrentPhase(EExcavationPhase::Completed);
}

void AExcavationManager::SetCurrentPhase(EExcavationPhase NewPhase)
{
	if (CurrentPhase == NewPhase) return;

	CurrentPhase = NewPhase;

	// 단계별 도구 가용성 업데이트
	switch (CurrentPhase)
	{
	case EExcavationPhase::Detection:
		ToolAvailabilityByPhase[0] = true;  // 탐지 도구
		ToolAvailabilityByPhase[1] = false; // 삽 도구
		ToolAvailabilityByPhase[2] = false; // 붓 도구
		ToolAvailabilityByPhase[3] = false; // 집게 도구
		break;

	case EExcavationPhase::Digging:
		ToolAvailabilityByPhase[0] = true;  // 탐지 도구 (여전히 사용 가능)
		ToolAvailabilityByPhase[1] = true;  // 삽 도구
		ToolAvailabilityByPhase[2] = false; // 붓 도구
		ToolAvailabilityByPhase[3] = false; // 집게 도구
		break;

	case EExcavationPhase::Brushing:
		ToolAvailabilityByPhase[0] = true;  // 탐지 도구
		ToolAvailabilityByPhase[1] = true;  // 삽 도구 (여전히 사용 가능)
		ToolAvailabilityByPhase[2] = true;  // 붓 도구
		ToolAvailabilityByPhase[3] = false; // 집게 도구
		break;

	case EExcavationPhase::Collection:
		ToolAvailabilityByPhase[0] = true;  // 탐지 도구
		ToolAvailabilityByPhase[1] = true;  // 삽 도구
		ToolAvailabilityByPhase[2] = true;  // 붓 도구
		ToolAvailabilityByPhase[3] = true;  // 집게 도구
		break;

	case EExcavationPhase::Completed:
		// 모든 도구 사용 가능 (완료 후 자유롭게 사용)
		ToolAvailabilityByPhase[0] = true;
		ToolAvailabilityByPhase[1] = true;
		ToolAvailabilityByPhase[2] = true;
		ToolAvailabilityByPhase[3] = true;
		break;
	}

	UE_LOG(LogTemp, Log, TEXT("[ExcavationManager] 발굴 단계 변경: %d"), (int32)CurrentPhase);
}

bool AExcavationManager::IsToolAvailableForPhase(int32 ToolIndex) const
{
	if (ToolIndex >= 0 && ToolIndex < ToolAvailabilityByPhase.Num())
	{
		return ToolAvailabilityByPhase[ToolIndex];
	}
	return false;
}

