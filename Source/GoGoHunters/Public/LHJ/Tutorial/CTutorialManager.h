#pragma once

#include "CoreMinimal.h"
#include "CTutorialData.h"
#include "GameFramework/Actor.h"
#include "GameFramework/SaveGame.h"
#include "CTutorialManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTutorialStepChanged, const FTutorialStepData&, StepData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTutorialCompleted);

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
	UFUNCTION(BlueprintCallable, Category = "Tutorial")
	void SkipTutorial();

	// 상태 확인
	UFUNCTION(BlueprintPure, Category = "Tutorial")
	bool IsTutorialActive() const { return bIsActive; }
	UFUNCTION(BlueprintPure, Category = "Tutorial")
	FTutorialStepData GetCurrentStep() const { return CurrentStep; }
	UFUNCTION(BlueprintPure, Category = "Tutorial")
	bool IsTutorialCompleted(const FString& StepID) const;

	// 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "Tutorial")
	FOnTutorialStepChanged OnTutorialStepChanged;
	UPROPERTY(BlueprintAssignable, Category = "Tutorial")
	FOnTutorialCompleted OnTutorialCompleted;

private:
	// 튜토리얼 데이터
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tutorial", meta = (AllowPrivateAccess))
	UDataTable* TutorialDataTable;

	// 현재 상태
	bool bIsActive = false;
	FString CurrentStepID;
	FTutorialStepData CurrentStep;

	// 완료된 튜토리얼 관리
	TMap<FString, bool> CompletedTutorials;
	void MarkTutorialCompleted(const FString& StepID);

	const FString TutorialSaveSlot = TEXT("TutorialSaveSlot");

public:
	ACTutorialManager();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
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