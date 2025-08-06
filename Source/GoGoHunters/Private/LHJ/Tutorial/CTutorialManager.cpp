#include "LHJ/Tutorial/CTutorialManager.h"
#include "UIs/Tutorial/CTutorialUI.h"
#include "Engine/DataTable.h"
#include "Kismet/GameplayStatics.h"

ACTutorialManager::ACTutorialManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ACTutorialManager::BeginPlay()
{
	Super::BeginPlay();
	LoadTutorialProgress(); // 시작 시 진행상황 불러오기
}

void ACTutorialManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACTutorialManager::StartTutorial(const FString& StepID)
{
	if (!TutorialDataTable) return;
	if (IsTutorialCompleted(StepID)) return; // 이미 완료된 튜토리얼이면 무시
	LoadTutorialStep(StepID);
	if (CurrentStep.StepID.IsEmpty()) return;
	bIsActive = true;
	CurrentStepID = StepID;
	OnTutorialStepChanged.Broadcast(CurrentStep); // 시작 델리게이트 호출
}

void ACTutorialManager::SkipTutorial()
{
	if (!bIsActive) return;
	bIsActive = false;
	EndTutorial(); // 스킵 시 바로 종료 처리(델리게이트 미사용)
}

void ACTutorialManager::StopTutorial()
{
	if (!bIsActive) return;
	bIsActive = false;
	EndTutorial(); // 정상 종료
}

void ACTutorialManager::EndTutorial()
{
	MarkTutorialCompleted(CurrentStepID); // 진행상황 저장
	SaveTutorialProgress();
	OnTutorialCompleted.Broadcast(); // 종료 델리게이트 호출
}

void ACTutorialManager::LoadTutorialStep(const FString& StepID)
{
	if (!TutorialDataTable) return;
	CurrentStep = FTutorialStepData();
	TArray<FName> RowNames = TutorialDataTable->GetRowNames();
	for (const FName& RowName : RowNames)
	{
		FTutorialStepData* Step = TutorialDataTable->FindRow<FTutorialStepData>(RowName, TEXT("TutorialLoad"));
		if (Step && Step->StepID == StepID)
		{
			CurrentStep = *Step;
			break;
		}
	}
}

bool ACTutorialManager::IsTutorialCompleted(const FString& StepID) const
{
	return CompletedTutorials.Contains(StepID) && CompletedTutorials[StepID];
}

void ACTutorialManager::MarkTutorialCompleted(const FString& StepID)
{
	CompletedTutorials.FindOrAdd(StepID) = true;
}

void ACTutorialManager::SaveTutorialProgress()
{
	UCTutorialSaveGame* SaveGameInstance = Cast<UCTutorialSaveGame>(UGameplayStatics::CreateSaveGameObject(UCTutorialSaveGame::StaticClass()));
	if (!SaveGameInstance) return;
	SaveGameInstance->CompletedTutorials = CompletedTutorials;
	UGameplayStatics::SaveGameToSlot(SaveGameInstance, TutorialSaveSlot, 0);
}

void ACTutorialManager::LoadTutorialProgress()
{
	if (UGameplayStatics::DoesSaveGameExist(TutorialSaveSlot, 0))
	{
		UCTutorialSaveGame* Loaded = Cast<UCTutorialSaveGame>(UGameplayStatics::LoadGameFromSlot(TutorialSaveSlot, 0));
		if (Loaded)
		{
			CompletedTutorials = Loaded->CompletedTutorials;
		}
	}
	else
	{
		CompletedTutorials.Empty();
	}
}
