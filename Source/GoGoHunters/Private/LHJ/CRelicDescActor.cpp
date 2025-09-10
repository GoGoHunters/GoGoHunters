#include "LHJ/CRelicDescActor.h"

#include "GameDelegates.h"
#include "Components/WidgetComponent.h"
#include "Components/TextRenderComponent.h"
#include "Utilities/CHelpers.h"

ACRelicDescActor::ACRelicDescActor()
{
	PrimaryActorTick.bCanEverTick = true;
	CHelpers::CreateComponent<UStaticMeshComponent>(this, &MeshComp, "MeshComp");
	MeshComp->SetCastShadow(false);

	CHelpers::CreateComponent<UTextRenderComponent>(this, &TitleTextComp, "TitleTextComp", RootComponent);
	TitleTextComp->SetRelativeLocation(FVector(6.5, 0, 12));
	TitleTextComp->SetHorizontalAlignment(EHTA_Center);
	TitleTextComp->SetVerticalAlignment(EVRTA_TextCenter);
	TitleTextComp->SetWorldSize(3.f);
	TitleTextComp->SetTextRenderColor(FColor(255, 155, 14, 1));
	
	CHelpers::CreateComponent<UTextRenderComponent>(this, &DescTextComp, "DescTextComp", RootComponent);
	DescTextComp->SetRelativeLocation(FVector(6.5, 0, 7));
	DescTextComp->SetHorizontalAlignment(EHTA_Center);
	DescTextComp->SetVerticalAlignment(EVRTA_TextTop);
	DescTextComp->SetWorldSize(1.3f);
	
	CHelpers::CreateComponent<UTextRenderComponent>(this, &CollectorTextComp, "CollectorTextComp", RootComponent);
	CollectorTextComp->SetRelativeLocation(FVector(6.5, 30, 22));
	CollectorTextComp->SetHorizontalAlignment(EHTA_Left);
	CollectorTextComp->SetVerticalAlignment(EVRTA_TextCenter);
	CollectorTextComp->SetWorldSize(1.f);
	
	CHelpers::CreateComponent<UTextRenderComponent>(this, &DateTextComp, "DateTextComp", RootComponent);
	DateTextComp->SetRelativeLocation(FVector(6.5, 30, 19));
	DateTextComp->SetHorizontalAlignment(EHTA_Left);
	DateTextComp->SetVerticalAlignment(EVRTA_TextCenter);
	DateTextComp->SetWorldSize(1.f);

	// CHelpers::CreateComponent<UWidgetComponent>(this, &DescWidget, "DescWidget", RootComponent);
	// DescWidget->SetCastShadow(false);
}

void ACRelicDescActor::UpdateDescriptionWidget(bool bUpdate, FCRelicData InRelicData,
                                               FCRelicDetailData InRelicDetailData)
{
	FCRelicDataParam param;
	param.RelicData = InRelicData;
	param.RelicDetailData = InRelicDetailData;
	param.IsUpdate = bUpdate;
	UpdateData(param);
	// UUserWidget* WBPDescriptionWidget = DescWidget->GetWidget();
	// if (!WBPDescriptionWidget) return;
	//
	// FName FunctionName(TEXT("UpdateData"));
	// UFunction* Function = WBPDescriptionWidget->FindFunction(FunctionName);
	// if (Function)
	// {
	// 	FCRelicDataParam param;
	// 	param.RelicData = InRelicData;
	// 	param.RelicDetailData = InRelicDetailData;
	// 	param.IsUpdate = bUpdate;
	// 	WBPDescriptionWidget->ProcessEvent(Function, &param);
	// }
}

void ACRelicDescActor::UpdateData(FCRelicDataParam Param)
{
	if (Param.IsUpdate)
	{
		VisibleComponent(true);
		TitleTextComp->SetText(Param.RelicDetailData.RelicName);
		FString FormattedDesc = FormatTextWithLineBreaks(Param.RelicDetailData.RelicDesc.ToString(), 30);
		DescTextComp->SetText(FText::FromString(FormattedDesc));

		if (Param.RelicData.CollectorName == NAME_None)
		{
			CollectorTextComp->SetText(FText::FromString(FString::Printf(TEXT("발굴 : 탐험가 미상"))));
		}
		else
		{
			CollectorTextComp->SetText(FText::FromString(FString::Printf(TEXT("발굴 : 탐험가 %s"), *Param.RelicData.CollectorName.ToString())));;
		}

		FText DateText = FText::AsDate(Param.RelicData.DropDate, EDateTimeStyle::Default, FText::GetInvariantTimeZone());
		
		DateTextComp->SetText(FText::FromString(FString::Printf(TEXT("날짜 : %s"), *DateText.ToString())));
	}
	else
	{
		VisibleComponent(false);
		DescTextComp->SetText(FText::FromString(FString::Printf(TEXT("전시 준비 중 입니다."))));		
	}
}

void ACRelicDescActor::VisibleComponent(bool bShown)
{
	TitleTextComp->SetVisibility(bShown);
	CollectorTextComp->SetVisibility(bShown);
	DateTextComp->SetVisibility(bShown);
}

FString ACRelicDescActor::FormatTextWithLineBreaks(const FString& Text, int32 MaxLength)
{
	if (Text.Len() <= MaxLength)
	{
		return Text;
	}
	
	FString Result;
	FString RemainingText = Text;
	
	while (RemainingText.Len() > MaxLength)
	{
		// MaxLength까지의 텍스트를 찾되, 공백이 있는 경우 가장 가까운 공백에서 자르기
		int32 CutPosition = MaxLength;
		for (int32 i = MaxLength; i >= 0; i--)
		{
			if (RemainingText[i] == TEXT(' '))
			{
				CutPosition = i;
				break;
			}
		}
		
		// 공백을 찾지 못한 경우 MaxLength에서 자르기
		if (CutPosition == MaxLength)
		{
			CutPosition = MaxLength;
		}
		
		Result += RemainingText.Left(CutPosition) + TEXT("\n");
		RemainingText = RemainingText.Mid(CutPosition + 1);
	}
	
	// 남은 텍스트가 있으면 추가
	if (RemainingText.Len() > 0)
	{
		Result += RemainingText;
	}
	
	return Result;
}
