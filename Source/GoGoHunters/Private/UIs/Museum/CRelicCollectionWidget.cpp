#include "UIs/Museum/CRelicCollectionWidget.h"

#include "base/GI_Base.h"
#include "Components/Button.h"
#include "Components/GridPanel.h"
#include "Components/GridSlot.h"
#include "Components/TextBlock.h"
#include "UIs/Museum/CRelicWidget.h"

void UCRelicCollectionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	GI = Cast<UGI_Base>(GetGameInstance());

	if (GI && InitRelicWidgets())
	{
		Txt_EmptyRelics->SetVisibility(ESlateVisibility::Hidden);
		Grid_Relics->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		Txt_EmptyRelics->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		Grid_Relics->SetVisibility(ESlateVisibility::Hidden);
	}

	Btn_Prev->OnClicked.AddDynamic(this, &UCRelicCollectionWidget::OnPrevPage);
	Btn_Later->OnClicked.AddDynamic(this, &UCRelicCollectionWidget::OnNextPage);
}

bool UCRelicCollectionWidget::InitRelicWidgets()
{
	Grid_Relics->ClearChildren();

	TArray<FCRelicData> RelicData = GI->GetAllRelicData();
	PlaceableRelics.Empty();
	for (const auto& Relic : RelicData)
	{
		if (Relic.IsPlace) continue;
		PlaceableRelics.Add(Relic);
	}

	CurrentPage = 0;
	MaxPage = FMath::CeilToInt((float)PlaceableRelics.Num() / ItemsPerPage);
	ShowPage(CurrentPage);

	return PlaceableRelics.Num() > 0;
}

void UCRelicCollectionWidget::ShowPage(int32 PageIndex)
{
	Grid_Relics->ClearChildren();

	const int32 Columns = 2;
	int32 StartIndex = PageIndex * ItemsPerPage;
	int32 EndIndex = FMath::Min(StartIndex + ItemsPerPage, PlaceableRelics.Num());
	int32 Row = 0;
	int32 Col = 0;

	for (int32 i = StartIndex; i < EndIndex; ++i)
	{
		if (!RelicWidgetFactory) break;
		UCRelicWidget* RelicWidget = CreateWidget<UCRelicWidget>(this, RelicWidgetFactory);
		if (!RelicWidget) break;
		RelicWidget->SetRelicData(PlaceableRelics[i]);
		UGridSlot* GridSlot = Grid_Relics->AddChildToGrid(RelicWidget, Row, Col);
		GridSlot->SetPadding(FMargin(10));

		if (++Col >= Columns)
		{
			Col = 0;
			++Row;
		}
	}

	UpdatePage();
}

void UCRelicCollectionWidget::UpdatePage()
{
	Txt_Page->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), CurrentPage + 1, MaxPage)));
}

void UCRelicCollectionWidget::OnNextPage()
{
	if (CurrentPage < MaxPage - 1)
	{
		++CurrentPage;
		ShowPage(CurrentPage);
	}
}

void UCRelicCollectionWidget::OnPrevPage()
{
	if (CurrentPage > 0)
	{
		--CurrentPage;
		ShowPage(CurrentPage);
	}
}
