#include "LHJ/CRelicDescActor.h"

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
	TitleTextComp->SetTranslucentSortPriority(2);
	
	CHelpers::CreateComponent<UTextRenderComponent>(this, &DescTextComp, "DescTextComp", RootComponent);
	DescTextComp->SetRelativeLocation(FVector(6.5, 0, 7));
	DescTextComp->SetHorizontalAlignment(EHTA_Center);
	DescTextComp->SetVerticalAlignment(EVRTA_TextTop);
	DescTextComp->SetWorldSize(1.3f);
	DescTextComp->SetTranslucentSortPriority(2);
	
	CHelpers::CreateComponent<UTextRenderComponent>(this, &CollectorTextComp, "CollectorTextComp", RootComponent);
	CollectorTextComp->SetRelativeLocation(FVector(6.5, 30, 22));
	CollectorTextComp->SetHorizontalAlignment(EHTA_Left);
	CollectorTextComp->SetVerticalAlignment(EVRTA_TextCenter);
	CollectorTextComp->SetWorldSize(1.f);
	CollectorTextComp->SetTranslucentSortPriority(2);
	
	CHelpers::CreateComponent<UTextRenderComponent>(this, &DateTextComp, "DateTextComp", RootComponent);
	DateTextComp->SetRelativeLocation(FVector(6.5, 30, 19));
	DateTextComp->SetHorizontalAlignment(EHTA_Left);
	DateTextComp->SetVerticalAlignment(EVRTA_TextCenter);
	DateTextComp->SetWorldSize(1.f);
	DateTextComp->SetTranslucentSortPriority(2);
}

void ACRelicDescActor::UpdateDescriptionWidget(bool bUpdate, FCRelicData InRelicData,
                                               FCRelicDetailData InRelicDetailData)
{
	FCRelicDataParam param;
	param.RelicData = InRelicData;
	param.RelicDetailData = InRelicDetailData;
	param.IsUpdate = bUpdate;
	UpdateData(param);
}

void ACRelicDescActor::UpdateData(FCRelicDataParam Param)
{
	if (Param.IsUpdate)
	{
		VisibleComponent(true);
		TitleTextComp->SetText(Param.RelicDetailData.RelicName);
		FString FormattedDesc = FormatTextWithLineBreaks(Param.RelicDetailData.RelicDesc.ToString(), 30);

		DescTextComp->SetWorldSize(1.4f);
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
		
		DescTextComp->SetWorldSize(3.2f);
		DescTextComp->SetText(FText::FromString(FString::Printf(TEXT("전시 준비 중 입니다"))));		
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
	// CR/LF 정규화 (실개행 유지). 리터럴 "\\n"은 아래 루프에서 줄바꿈으로 처리
	FString Normalized = Text;
	Normalized.ReplaceInline(TEXT("\r\n"), TEXT("\n"));
	Normalized.ReplaceInline(TEXT("\r"), TEXT(""));

	FString Result;
	FString CurrentLine;

	int32 idx = 0;
	while (idx < Normalized.Len())
	{
		const TCHAR ch = Normalized[idx];

		// 리터럴 "\\n" 처리: 출력하지 않고 줄바꿈, 카운트 초기화
		if (ch == TEXT('\\') && (idx + 1) < Normalized.Len() && Normalized[idx + 1] == TEXT('n'))
		{
			if (CurrentLine.Len() > 0)
			{
				Result += CurrentLine;
				CurrentLine.Empty();
			}
			Result += TEXT("\n");
			idx += 2;
			continue;
		}

		// 실제 개행 처리
		if (ch == TEXT('\n'))
		{
			if (CurrentLine.Len() > 0)
			{
				Result += CurrentLine;
				CurrentLine.Empty();
			}
			Result += TEXT("\n");
			++idx;
			continue;
		}

		CurrentLine.AppendChar(ch);

		// 최대 길이에 도달 시 단어 기준 줄바꿈, 카운트 초기화
		if (CurrentLine.Len() >= MaxLength)
		{
			int32 SpaceIndex = INDEX_NONE;
			for (int32 si = CurrentLine.Len() - 1; si >= 0; --si)
			{
				if (CurrentLine[si] == TEXT(' '))
				{
					SpaceIndex = si;
					break;
				}
			}

			if (SpaceIndex != INDEX_NONE)
			{
				Result += CurrentLine.Left(SpaceIndex);
				Result += TEXT("\n");
				CurrentLine = CurrentLine.Mid(SpaceIndex + 1);
				CurrentLine = CurrentLine.TrimStart();
			}
			else
			{
				Result += CurrentLine.Left(MaxLength);
				Result += TEXT("\n");
				CurrentLine = CurrentLine.Mid(MaxLength);
			}
		}

		++idx;
	}

	if (CurrentLine.Len() > 0)
	{
		Result += CurrentLine;
	}

	return Result;
}
