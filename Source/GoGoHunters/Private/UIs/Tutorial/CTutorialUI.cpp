#include "UIs/Tutorial/CTutorialUI.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

void UCTutorialUI::NativeConstruct()
{
	Super::NativeConstruct();
	if (SkipButton)
		SkipButton->OnClicked.AddDynamic(this, &UCTutorialUI::OnSkipButtonClicked);
}

void UCTutorialUI::ShowTutorial(const FTutorialStepData& StepData)
{
	CurrentStepData = StepData;
	if (TutorialImage && StepData.TutorialImage)
		TutorialImage->SetBrushFromTexture(StepData.TutorialImage);
	PlayTutorialAnimation(StepData.TutorialAnimation);
	PlayTutorialSound(StepData.TutorialSound);
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UCTutorialUI::HideTutorial()
{
	SetVisibility(ESlateVisibility::Collapsed);
}

void UCTutorialUI::OnSkipButtonClicked()
{
	// 매니저에 SkipTutorial 호출 등
}

void UCTutorialUI::PlayTutorialAnimation(UAnimationAsset* Animation)
{
	// 애니메이션 재생 로직 (예: UMG 애니메이션, 캐릭터 애니메이션 등)
}

void UCTutorialUI::PlayTutorialSound(USoundBase* Sound)
{
	if (Sound)
		UGameplayStatics::PlaySound2D(this, Sound);
}

