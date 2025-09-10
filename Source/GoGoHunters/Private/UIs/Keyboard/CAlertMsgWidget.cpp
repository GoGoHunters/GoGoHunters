#include "UIs/Keyboard/CAlertMsgWidget.h"

#include "EngineUtils.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "Keyboard/AC_KeyBoard.h"
#include "LHM/Excavation/ExcavationManager.h"

void UCAlertMsgWidget::NativeConstruct()
{
	Super::NativeConstruct();
	BtnOk->OnClicked.AddDynamic(this, &UCAlertMsgWidget::UCAlertMsgWidget::OnClickBtnOk);
	BtnNo->OnClicked.AddDynamic(this, &UCAlertMsgWidget::UCAlertMsgWidget::OnClickClose);
	BtnClose->OnClicked.AddDynamic(this, &UCAlertMsgWidget::UCAlertMsgWidget::OnClickClose);
	BtnClose2->OnClicked.AddDynamic(this, &UCAlertMsgWidget::UCAlertMsgWidget::OnClickClose);
}

void UCAlertMsgWidget::SetInitialText(const FString InText)
{
	InitialText = InText;
	TxtInputText->SetText(FText::FromString(InitialText));
	TxtInputText2->SetText(FText::FromString(InitialText));
	TxtInputText3->SetText(FText::FromString(InitialText));
	Switcher->SetActiveWidgetIndex(0);
}

void UCAlertMsgWidget::OnClickBtnOk()
{
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, *FString("dfdf"));
	if (bRequest) return;
	bRequest = true;
	for (TActorIterator<AExcavationManager> It(GetWorld()); It; ++It)
	{
		if ((*It)->RegisterRelicCollector(InitialText))
			Switcher->SetActiveWidgetIndex(1);
		else
			Switcher->SetActiveWidgetIndex(2);

		break;
	}
	bRequest = false;
}

void UCAlertMsgWidget::OnClickClose()
{
	if (bRequest) return;
	KeyBoard->FinishEnterPlayerInitial();
}
