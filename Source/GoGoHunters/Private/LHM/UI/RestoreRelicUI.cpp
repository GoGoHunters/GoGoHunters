// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/UI/RestoreRelicUI.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void URestoreRelicUI::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (Btn_Relic) Btn_Relic->OnClicked.AddDynamic(this, &URestoreRelicUI::HandleButtonClicked);
}

void URestoreRelicUI::InitItem(const FCRelicData& InData)
{
	if(!Txt_RelicName) return;
	
	RelicData = InData;

	FText RelicName = RelicData.RelicName.IsEmpty() ? FText::FromString("NULL") : RelicData.RelicName;
	Txt_RelicName->SetText(RelicName);

	/*FSlateBrush NewBrush = Img_Relic->GetBrush();
	if (RelicData && RelicData->RelicImage)
	{
		NewBrush.SetResourceObject(RelicDetailData->RelicImage);
		Img_Relic->SetBrush(NewBrush);
	}*/
}

void URestoreRelicUI::HandleButtonClicked()
{
	OnRelicItemClicked.Broadcast(RelicData);
}
