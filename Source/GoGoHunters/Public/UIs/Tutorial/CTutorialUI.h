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
	void ShowTutorial(const FTutorialStepData& StepData);

	UFUNCTION(BlueprintCallable, Category = "Tutorial UI")
	void HideTutorial();

private:
	virtual void NativeConstruct() override;

	// UI 컴포넌트들
	UPROPERTY(meta = (BindWidget))
	UImage* TutorialImage;
	UPROPERTY(meta = (BindWidget))
	UButton* SkipButton;

	FTutorialStepData CurrentStepData;

	// 버튼 이벤트
	UFUNCTION()
	void OnSkipButtonClicked();

	// 애니메이션 및 사운드
	void PlayTutorialAnimation(UAnimationAsset* Animation);
	void PlayTutorialSound(USoundBase* Sound);
};
