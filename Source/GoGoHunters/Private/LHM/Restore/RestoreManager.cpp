// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/Restore/RestoreManager.h"
#include "LHM/UI/RestoreUI.h"
#include "base/GI_Base.h"
#include "LHM/Restore/RestorePuzzleActor.h"
#include "LHM/Restore/RestorWidgetActor.h"
#include "EngineUtils.h"
#include "LHM/UI/RestorationCompleteUI.h"
#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"

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

	for (TActorIterator<AActor> It(GetWorld(), AActor::StaticClass()); It; ++It)
	{
		if (IsValid(*It) && (*It)->ActorHasTag(FName("BabyDino")))
		{
			BabyDino = *It;
			BabyDino->SetActorHiddenInGame(true);
			break;
		}
	}

	// 시퀀스 이벤트 바인딩
	if (CompletionSequenceActor && CompletionSequenceActor->GetSequencePlayer())
	{
		CompletionSequenceActor->GetSequencePlayer()->OnFinished.AddDynamic(this, &ARestoreManager::OnSequenceEndPlay);
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

	// 1. 유물 데이터 저장 요청
	if (UGI_Base* GI = GetGameInstance<UGI_Base>())
	{
		FRelicSaveData NewSaveData;
		NewSaveData.RelicData = PuzzleActor->GetRelicData();
		GI->SaveRelicData(NewSaveData);
	}

	// 2. 완성 시퀀스 플레이
	PlayCompleteSequence();
	 
	// 3. 스탬프 애니메이션
	// 4. AI 타미 대사
}

void ARestoreManager::PlayCompleteSequence()
{
	if (BabyDino)
	{
		BabyDino->SetActorHiddenInGame(false);
	}

	if (CompletionSequenceActor && CompletionSequenceActor->GetSequencePlayer())
	{
		CompletionSequenceActor->GetSequencePlayer()->Play();
	}
}

void ARestoreManager::OnSequenceEndPlay()
{
	// 시퀀스가 끝나면 박물관 레벨 이동 UI 가시화
	if (CompleteUI) 
	{
		CompleteUI->SetCompleteVisibility(true);
	}
}