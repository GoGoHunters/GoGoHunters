#include "LHJ/CContinentWidgetActor.h"

#include "Components/TextRenderComponent.h"
#include "Components/WidgetComponent.h"
#include "LHJ/CWorldMap.h"
#include "UIs/WorldMap/CContinentWidget.h"
#include "Utilities/CHelpers.h"

// 로컬 헬퍼: \n 리터럴은 출력하지 않고 줄바꿈, 실제 개행도 처리.
// 단어 기준 줄바꿈(공백 우선), 없으면 강제 줄바꿈. 기본 25자.
static FString FormatTextWithLineBreaks_Local(const FString& Text, int32 MaxLength = 25)
{
	FString Normalized = Text;
	Normalized.ReplaceInline(TEXT("\r\n"), TEXT("\n"));
	Normalized.ReplaceInline(TEXT("\r"), TEXT(""));

	FString Result;
	FString CurrentLine;

	int32 idx = 0;
	while (idx < Normalized.Len())
	{
		const TCHAR ch = Normalized[idx];

		// 리터럴 "\\n" 처리
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

		// 실제 개행
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

ACContinentWidgetActor::ACContinentWidgetActor()
{
	PrimaryActorTick.bCanEverTick = true;
	CHelpers::CreateComponent<USceneComponent>(this, &RootScene, "RootScene");
	CHelpers::CreateComponent<UWidgetComponent>(this, &WidgetComponent, "WidgetComponent", RootComponent);
	WidgetComponent->SetCollisionProfileName(FName("VRUI"));

	CHelpers::CreateComponent<UTextRenderComponent>(this, &TitleTextComp, "TitleTextComp", RootComponent);
	TitleTextComp->SetRelativeLocationAndRotation(FVector(0,41,0),FRotator(90,0,90));
	TitleTextComp->SetHorizontalAlignment(EHTA_Center);
	TitleTextComp->SetVerticalAlignment(EVRTA_TextCenter);
	TitleTextComp->SetWorldSize(4.5f);
	TitleTextComp->SetTextRenderColor(FColor(255, 155, 14, 1));
	TitleTextComp->SetTranslucentSortPriority(2);
	
	CHelpers::CreateComponent<UTextRenderComponent>(this, &DescTextComp, "DescTextComp", RootComponent);
	DescTextComp->SetRelativeLocationAndRotation(FVector(73,30.5,0),FRotator(90,0,90));
	DescTextComp->SetHorizontalAlignment(EHTA_Left);
	DescTextComp->SetVerticalAlignment(EVRTA_TextTop);
	DescTextComp->SetWorldSize(2.5f);
	DescTextComp->SetTranslucentSortPriority(2);

	CHelpers::CreateComponent<UTextRenderComponent>(this, &RelicsTextComp, "RelicsTextComp", RootComponent);
	RelicsTextComp->SetRelativeLocationAndRotation(FVector(48,30.5,0),FRotator(90,0,90));
	RelicsTextComp->SetHorizontalAlignment(EHTA_Left);
	RelicsTextComp->SetVerticalAlignment(EVRTA_TextTop);
	RelicsTextComp->SetWorldSize(2.5f);
	RelicsTextComp->SetTranslucentSortPriority(2);
	
	CHelpers::CreateComponent<UTextRenderComponent>(this, &MoveTextComp, "MoveTextComp", RootComponent);
	MoveTextComp->SetRelativeLocationAndRotation(FVector(21.5,-44.2,0),FRotator(90,0,90));
	MoveTextComp->SetHorizontalAlignment(EHTA_Center);
	MoveTextComp->SetVerticalAlignment(EVRTA_TextCenter);
	MoveTextComp->SetWorldSize(3.f);
	MoveTextComp->SetTextRenderColor(FColor(0, 0, 0, 1));
	MoveTextComp->SetTranslucentSortPriority(2);
	
	CHelpers::CreateComponent<UTextRenderComponent>(this, &CloseTextComp, "CloseTextComp", RootComponent);
	CloseTextComp->SetRelativeLocationAndRotation(CloseTextLoc1,FRotator(90,0,90));
	CloseTextComp->SetHorizontalAlignment(EHTA_Center);
	CloseTextComp->SetVerticalAlignment(EVRTA_TextCenter);
	CloseTextComp->SetWorldSize(3.f);
	CloseTextComp->SetTextRenderColor(FColor(0, 0, 0, 1));
	CloseTextComp->SetTranslucentSortPriority(2);	
}

void ACContinentWidgetActor::BeginPlay()
{
	Super::BeginPlay();
	
	if (WidgetComponent->GetWidget())
	{
		ContinentWidget = Cast<UCContinentWidget>(WidgetComponent->GetWidget());
		ContinentWidget->SetOuterActor(this);
	}
}

void ACContinentWidgetActor::SetContinentData(const FCContinentData& ContinentData)
{
	if (!ContinentWidget) return;
	ContinentWidget->SetContinentData(ContinentData);

	TitleTextComp->SetText(FText::AsCultureInvariant(ContinentData.ContinentName));

	FString strDesc = "";
	if (!ContinentData.ContinentDesc.TrimStartAndEnd().IsEmpty()) 
	{
		strDesc += FormatTextWithLineBreaks_Local(ContinentData.ContinentDesc, 40);
	}

	// 리터럴 "\\n"을 실제 줄바꿈으로 변환하여 출력
	strDesc.ReplaceInline(TEXT("\r\n"), TEXT("\n"));
	strDesc.ReplaceInline(TEXT("\r"), TEXT(""));
	strDesc.ReplaceInline(TEXT("\\n"), TEXT("\n"));
	DescTextComp->SetText(FText::AsCultureInvariant(strDesc));

	FString strRelics = "";
	if (ContinentData.RelicsArray.Num() > 0)
	{
		strRelics += TEXT("등장 유물\n");
		for (const auto& Relic : ContinentData.RelicsArray) 
		{
			strRelics += TEXT("※") + Relic + TEXT("\n");
		}
	}

	// 리터럴 "\\n"을 실제 줄바꿈으로 변환하여 출력
	strRelics.ReplaceInline(TEXT("\r\n"), TEXT("\n"));
	strRelics.ReplaceInline(TEXT("\r"), TEXT(""));
	strRelics.ReplaceInline(TEXT("\\n"), TEXT("\n"));
	RelicsTextComp->SetText(FText::AsCultureInvariant(strRelics));

	if (!ContinentData.UseJoin)
	{
		MoveTextComp->SetVisibility(false);
		CloseTextComp->SetRelativeLocation(CloseTextLoc2);
	}
	else
	{
		MoveTextComp->SetVisibility(true);
		CloseTextComp->SetRelativeLocation(CloseTextLoc1);
	}
}

void ACContinentWidgetActor::SetContinentVisibleHidden()
{
	if (!OuterOwner) return;
	OuterOwner->SetContinentVisibleHidden();
}

void ACContinentWidgetActor::SetOuterActor(ACWorldMap* InOwner)
{
	if (!InOwner) return;
	OuterOwner = InOwner;
}
