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

	if (!ActivePuzzleActor)
	{
		TArray<AActor*> Found;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARestorePuzzleActor::StaticClass(), Found);

		if (Found.Num() > 0)
		{
			ActivePuzzleActor = Cast<ARestorePuzzleActor>(Found[0]);
		}
	}
}

void ARestoreManager::StartRestoration(const FCRelicData& RelicData)
{
	CurrentRelicData = RelicData;
	SpawnPuzzleActor(RelicData);
}

void ARestoreManager::SpawnPuzzleActor(const FCRelicData& RelicData)
{
	ActivePuzzleActor->InitPuzzle(RelicData, this);
}

void ARestoreManager::NotifyPuzzleCompleted(class ARestorePuzzleActor* PuzzleActor)
{
	if (!PuzzleActor) return;

	const FCRelicData& CompletedRelic = PuzzleActor->GetRelicData();

	// 1. 유물 데이터 저장 요청
	if (UGI_Base* GI = GetGameInstance<UGI_Base>())
	{
		FRelicSaveData NewSaveData;
		NewSaveData.RelicData = CompletedRelic;
		GI->SaveRelicData(NewSaveData);
	}

	// 2. 완료 UI 표시 예정
	// 3. 스탬프 애니메이션 예정
	// 4. AI 타미 대사 예정
}

