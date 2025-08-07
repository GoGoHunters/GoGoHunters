#pragma once

#include "CoreMinimal.h"
#include "CTutorialData.h"
#include "GameFramework/Actor.h"
#include "GameFramework/SaveGame.h"
#include "CTutorialManager.generated.h"

UCLASS()
class GOGOHUNTERS_API ACTutorialManager : public AActor
{
	GENERATED_BODY()
	
public:
	// 튜토리얼 시작/중지
	UFUNCTION(BlueprintCallable, Category = "Tutorial")
	void StartTutorial(const FString& StepID);
	UFUNCTION(BlueprintCallable, Category = "Tutorial")
	void StopTutorial();

	// 상태 확인
	UFUNCTION(BlueprintPure, Category = "Tutorial")
	bool IsTutorialActive() const { return bIsActive; }
	UFUNCTION(BlueprintPure, Category = "Tutorial")
	FCTutorialData GetCurrentStep() const { return CurrentStep; }
	UFUNCTION(BlueprintPure, Category = "Tutorial")
	bool IsTutorialCompleted(const FString& StepID) const;

private:
	// 튜토리얼 데이터
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tutorial", meta = (AllowPrivateAccess))
	UDataTable* TutorialDataTable;

	// 현재 상태
	bool bIsActive = false;
	FString CurrentStepID;
	FCTutorialData CurrentStep;

	// 완료된 튜토리얼 관리
	UPROPERTY(VisibleInstanceOnly)
	TMap<FString, bool> CompletedTutorials;

	UPROPERTY()
	TObjectPtr<APawn> TamiAI;
	
	void MarkTutorialCompleted(const FString& StepID);

	const FString TutorialSaveSlot = TEXT("TutorialSaveSlot");

	ACTutorialManager();
	virtual void BeginPlay() override;
	
	void LoadTutorialStep(const FString& StepID);
	void EndTutorial();
	void SaveTutorialProgress();
	void LoadTutorialProgress();
};

// 튜토리얼 저장용 SaveGame 클래스 정의
UCLASS()
class UCTutorialSaveGame : public USaveGame
{
	GENERATED_BODY()
public:
	UPROPERTY()
	TMap<FString, bool> CompletedTutorials;
};