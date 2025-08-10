#include "LHJ/Tutorial/CTutorialManager.h"

#include "EngineUtils.h"
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

	for (TActorIterator<APawn> It(GetWorld(), APawn::StaticClass()); It; ++It)
	{
		if (IsValid(*It) && (*It)->ActorHasTag(FName("Tami")))
		{
			TamiAI = *It;
			break;
		}
	}
}

void ACTutorialManager::StartTutorial(const FString& StepID)
{
	if (!TutorialDataTable) return;
	if (IsTutorialCompleted(StepID)) return; // 이미 완료된 튜토리얼이면 무시
	LoadTutorialStep(StepID);
	if (CurrentStep.StepID.IsEmpty()) return;
	bIsActive = true;
	CurrentStepID = StepID;

	if (TamiAI)
	{
		// 블루프린트 함수 이름
		FName FunctionName(TEXT("StartTutorial")); // 블루프린트에서 정의한 함수명

		// 블루프린트 함수 가져오기
		UFunction* Function = TamiAI->FindFunction(FunctionName);
		if (Function)
			// 블루프린트 함수 호출 (매개변수가 있는 경우)
			TamiAI->ProcessEvent(Function, &CurrentStep);
	}	
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

	// 키 가이드가 종료되면 메뉴 가이드 바로 실행
	if (CurrentStepID == "KEY_GUIDE")
	{
		StartTutorial("MENU_GUIDE");
	}
}

void ACTutorialManager::LoadTutorialStep(const FString& StepID)
{
	if (!TutorialDataTable) return;
	CurrentStep = FCTutorialData();
	TArray<FName> RowNames = TutorialDataTable->GetRowNames();
	for (const FName& RowName : RowNames)
	{
		FCTutorialData* Step = TutorialDataTable->FindRow<FCTutorialData>(RowName, TEXT("TutorialLoad"));
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
