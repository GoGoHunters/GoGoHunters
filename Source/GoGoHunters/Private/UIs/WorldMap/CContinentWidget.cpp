#include "UIs/WorldMap/CContinentWidget.h"

#include "Components/Button.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "LHJ/CContinentData.h"
#include "Engine/World.h"
#include "base/GI_Base.h"

void UCContinentWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	// Join 버튼 클릭 이벤트 바인딩
	if (Btn_Join)
	{
		Btn_Join->OnClicked.AddDynamic(this, &UCContinentWidget::OnJoinButtonClicked);
	}
}

void UCContinentWidget::SetContinentData(const FCContinentData& ContinentData)
{
	// 현재 대륙 데이터 저장
	CurrentContinentData = ContinentData;
	
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

void UCContinentWidget::OnJoinButtonClicked()
{
	// GameInstance를 통해 레벨 전환
	if (UGI_Base* GameInstance = Cast<UGI_Base>(GetGameInstance()))
	{
		// 현재 대륙의 타겟 레벨로 이동
		if (!CurrentContinentData.TargetLevelName.IsEmpty())
		{
			GameInstance->TransitionToLevel(CurrentContinentData.TargetLevelName);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Target level name is empty for continent: %s"), *CurrentContinentData.ContinentName);
		}
	}
}
