#include "UIs/WorldMap/CContinentWidget.h"

#include "Components/Button.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "LHJ/CContinentData.h"

void UCContinentWidget::SetContinentData(const FCContinentData& ContinentData)
{
	if (!ContinentData.ContinentName.TrimStartAndEnd().IsEmpty()) 
	{
		Txt_Title->SetText(FText::AsCultureInvariant(ContinentData.ContinentName));
	}

	FString strDesc = "";
	if (!ContinentData.ContinentDesc.TrimStartAndEnd().IsEmpty()) 
	{
		strDesc += ContinentData.ContinentDesc;
	}
	
	if (ContinentData.RelicsArray.Num() > 0)
	{
		strDesc += TEXT("\r\n\r\n등장 유물\r\n");
		for (const auto& Relic : ContinentData.RelicsArray) 
		{
			strDesc += TEXT("※") + Relic + TEXT("\r\n");
		}
	}
	
	Txt_Desc->SetText(FText::AsCultureInvariant(strDesc));

	if (!ContinentData.UseJoin)
	{
		Btn_Join->SetVisibility(ESlateVisibility::Hidden);
		SB_Exit->SetVisibility(ESlateVisibility::Hidden);
		SB_Exit_NotUseJoin->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		Btn_Join->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SB_Exit->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SB_Exit_NotUseJoin->SetVisibility(ESlateVisibility::Hidden);
	}
}
