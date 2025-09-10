
#include "UIs/CollectingBook/CCollectingBookWidget.h"
#include "base/GI_Base.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Components/WidgetSwitcher.h"
#include "UIs/CollectingBook/CCollectingItem.h"
#include "Containers/UnrealString.h"

// 리터럴 "\\n"을 실제 줄바꿈으로 변환하고 CRLF를 정규화
static FString NormalizeNewlines(const FString& In)
{
	FString S = In;
	S.ReplaceInline(TEXT("\r\n"), TEXT("\n"));
	S.ReplaceInline(TEXT("\r"), TEXT(""));
	S.ReplaceInline(TEXT("\\n"), TEXT("\n"));
	return S;
}

void UCCollectingBookWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Btn_Prev->OnClicked.AddDynamic(this, &UCCollectingBookWidget::OnPrevPage);
	Btn_Later->OnClicked.AddDynamic(this, &UCCollectingBookWidget::OnNextPage);
	Btn_Back->OnClicked.AddDynamic(this, &UCCollectingBookWidget::GoToList);
	Btn_Back_1->OnClicked.AddDynamic(this, &UCCollectingBookWidget::GoToList);
	
	GI = Cast<UGI_Base>(GetGameInstance());
}

void UCCollectingBookWidget::InitData()
{
	WS_Main->SetActiveWidgetIndex(0);

	CollectingData.Empty();
	for (TPair<int32, FCRelicCollectingBook>& Pair : GI->GetRelicCollectingData())
	{
		CollectingData.Add(Pair.Value);
	}

	CurrentPage = 0;
	MaxPage = FMath::CeilToInt((float)CollectingData.Num() / ItemsPerPage);
	ShowPage(CurrentPage);
}

void UCCollectingBookWidget::ShowPage(int32 PageIndex)
{
	Grid_Relics->ClearChildren();

	int32 StartIndex = PageIndex * ItemsPerPage;
	int32 EndIndex = FMath::Min(StartIndex + ItemsPerPage, CollectingData.Num());
	int32 Row = 0;
	int32 Col = 0;

	for (int32 i = StartIndex; i < EndIndex; ++i)
	{
		if (!CollectingItemWidgetFactory) break;
		UCCollectingItem* CollectingItem = CreateWidget<UCCollectingItem>(this, CollectingItemWidgetFactory);
		if (!CollectingItem) break;
		CollectingItem->SetData(CollectingData[i]);
		CollectingItem->SetCollectionWidget(this);
		UUniformGridSlot* GridSlot = Grid_Relics->AddChildToUniformGrid(CollectingItem, Row, Col);
		GridSlot->SetHorizontalAlignment(HAlign_Center);
		GridSlot->SetVerticalAlignment(VAlign_Center);

		if (++Col >= Columns)
		{
			Col = 0;
			++Row;
		}
	}

	UpdatePage();
}

void UCCollectingBookWidget::UpdatePage()
{
	Txt_Page->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), CurrentPage + 1, MaxPage)));
}

void UCCollectingBookWidget::GoToList()
{
	WS_Main->SetActiveWidgetIndex(0);
}

void UCCollectingBookWidget::ShowRelicDetailPage(const FCRelicCollectingBook& InRelicCollectingBookData)
{
	if (InRelicCollectingBookData.IsDrop)
	{
		WS_Main->SetActiveWidgetIndex(1);

		if (InRelicCollectingBookData.RelicDetailData.RelicImage)
		{
			FSlateBrush newBrush = Img_Thumbnail->GetBrush();
			newBrush.SetResourceObject(InRelicCollectingBookData.RelicDetailData.RelicImage);
			Img_Thumbnail->SetBrush(newBrush);			
		}

		Txt_RelicName->SetText(InRelicCollectingBookData.RelicDetailData.RelicName);
		Txt_Continent->SetText(InRelicCollectingBookData.RelicDetailData.ContinentName);

		FText FormattedDateText = FText::FromString(InRelicCollectingBookData.DropDate.ToString(TEXT("%Y-%m-%d")));
		Txt_DropDate->SetText(FormattedDateText);

		Txt_Desc->SetText(FText::FromString(NormalizeNewlines(InRelicCollectingBookData.RelicDetailData.RelicDesc.ToString())));
	}
	else
	{
		WS_Main->SetActiveWidgetIndex(2);

		if (InRelicCollectingBookData.RelicDetailData.RelicBlindImage)
		{
			FSlateBrush newBrush = Img_Thumbnail->GetBrush();
			newBrush.SetResourceObject(InRelicCollectingBookData.RelicDetailData.RelicBlindImage);
			Img_Thumbnail->SetBrush(newBrush);			
		}
		
		Txt_HiddenDesc->SetText(InRelicCollectingBookData.RelicDetailData.RelicToolTipDesc);
	}
}

void UCCollectingBookWidget::OnPrevPage()
{
	if (CurrentPage > 0)
	{
		--CurrentPage;
		ShowPage(CurrentPage);
	}
}

void UCCollectingBookWidget::OnNextPage()
{
	if (CurrentPage < MaxPage - 1)
	{
		++CurrentPage;
		ShowPage(CurrentPage);
	}
}