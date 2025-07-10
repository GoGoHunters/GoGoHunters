#include "UIs/Museum/CRelicWidget.h"

#include "base/GI_Base.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "JMH/CMuseumComponent.h"
#include "JMH/MH_VRPlayer.h"
#include "LHJ/CRelicBase.h"

void UCRelicWidget::NativeConstruct()
{
	Super::NativeConstruct();

	GI = Cast<UGI_Base>(GetGameInstance());
	if (GI) SetRelicDetailData();

	Btn_SelectRelic->OnClicked.AddDynamic(this, &UCRelicWidget::OnSelectRelicButtonClicked);
}

void UCRelicWidget::SetRelicDetailData()
{
	if (RelicData.RelicName.EqualTo(FText::FromString(""))) return;

	RelicDetailData = GI->GetRelicDetailDataByName(RelicData.RelicName.ToString());

	Txt_RelicName->SetText(RelicData.RelicName);
	FSlateBrush NewBrush = Img_Relic->GetBrush();

	if (RelicDetailData->RelicImage)
	{
		NewBrush.SetResourceObject(RelicDetailData->RelicImage);
		Img_Relic->SetBrush(NewBrush);
	}
}

void UCRelicWidget::OnSelectRelicButtonClicked()
{
	AMH_VRPlayer* Player=Cast<AMH_VRPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (!Player) return;
	UCMuseumComponent* MuseumComponent=Player->GetComponentByClass<UCMuseumComponent>();
	if (!MuseumComponent) return;
	// Player 손에 액터를 스폰시킨다
	MuseumComponent->PlayPreviewMode(RelicData, *RelicDetailData);	
}
