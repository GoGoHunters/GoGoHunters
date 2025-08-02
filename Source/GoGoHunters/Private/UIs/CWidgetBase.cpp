#include "UIs/CWidgetBase.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "JMH/CMuseumComponent.h"
#include "UIs/CUiActor.h"

class UCMuseumComponent;

void UCWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();
	if (Btn_A)
		Btn_A->OnClicked.AddDynamic(this, &UCWidgetBase::OnBtnAClicked);
	if (Btn_B)
		Btn_B->OnClicked.AddDynamic(this, &UCWidgetBase::OnBtnBClicked);
}

void UCWidgetBase::OnBtnAClicked()
{
	SwitchUI(0);
}

void UCWidgetBase::OnBtnBClicked()
{
	SwitchUI(1);
}

void UCWidgetBase::SwitchUI(const int32 InDesiredUiIdx)
{
	if (!UiActor) return;
	UiActor->SwitchActiveWidget(InDesiredUiIdx);
}

void UCWidgetBase::BlindActive(bool bUseBlind)
{
	if (bUseBlind)
		Img_Blind->SetVisibility(ESlateVisibility::Visible);
	else
		Img_Blind->SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UCWidgetBase::K2_PlayUiAnim_Implementation(bool bIsReverse)
{
}
