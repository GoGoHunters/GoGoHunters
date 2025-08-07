#include "UIs/Tutorial/CTutorialUI.h"

#include "Components/Button.h"
#include "Components/Image.h"

void UCTutorialUI::NativeConstruct()
{
	Super::NativeConstruct();
	if (BtnSkip)
		BtnSkip->OnClicked.AddDynamic(this, &UCTutorialUI::OnSkipButtonClicked);
}

void UCTutorialUI::ShowTutorial(const FCTutorialData& StepData)
{
	CurrentStepData = StepData;
	if (ImgTutorial && StepData.TutorialImage)
		ImgTutorial->SetBrushFromTexture(StepData.TutorialImage);
}

void UCTutorialUI::EndTutorial()
{
	if (ImgTutorial)
		ImgTutorial->SetBrushFromTexture(nullptr);
	CurrentStepData = FCTutorialData();
}

void UCTutorialUI::OnSkipButtonClicked()
{
	if (!AI_Pawn) return;
	
	// 블루프린트 함수 이름
	FName FunctionName(TEXT("SkipTutorial")); // 블루프린트에서 정의한 함수명

	// 블루프린트 함수 가져오기
	UFunction* Function = AI_Pawn->FindFunction(FunctionName);
	if (Function)
		// 블루프린트 함수 호출 (매개변수가 없는 경우)
		AI_Pawn->ProcessEvent(Function, nullptr);
}
