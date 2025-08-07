#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LHJ/Tutorial/CTutorialData.h"
#include "CTutorialUI.generated.h"

class UButton;
class UTextBlock;
class UImage;
/**
 * 
 */
UCLASS()
class GOGOHUNTERS_API UCTutorialUI : public UUserWidget
{
	GENERATED_BODY()
public:
	// 튜토리얼 표시
	UFUNCTION(BlueprintCallable, Category = "Tutorial UI")
	void ShowTutorial(const FCTutorialData& StepData);
	UFUNCTION(BlueprintCallable, Category = "Tutorial UI")
	void EndTutorial();

	UFUNCTION(BlueprintCallable)
	void SetAI_Pawn(APawn* InPawn) { AI_Pawn = InPawn; }

private:
	virtual void NativeConstruct() override;

	// UI 컴포넌트들
	UPROPERTY(meta = (BindWidget))
	UImage* ImgTutorial;
	UPROPERTY(meta = (BindWidget))
	UButton* BtnSkip;

	FCTutorialData CurrentStepData;

	UPROPERTY()
	TObjectPtr<APawn> AI_Pawn;
	
	// 버튼 이벤트
	UFUNCTION()
	void OnSkipButtonClicked();
};
