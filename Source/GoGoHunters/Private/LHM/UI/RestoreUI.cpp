// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/UI/RestoreUI.h"
#include "Components/HorizontalBox.h"
#include "LHM/UI/RestoreRelicUI.h"
#include "LHM/Restore/RestoreManager.h"
#include "base/GI_Base.h"
#include "Components/HorizontalBoxSlot.h"
#include "EngineUtils.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/Image.h"

void URestoreUI::NativeConstruct()
{
	Super::NativeConstruct();

	RelicSlotOverlays = {
		Overlay1_1, Overlay1_2, Overlay1_3,
		Overlay2_1, Overlay2_2, Overlay2_3,
		Overlay3_1, Overlay3_2, Overlay3_3
	};

	if (UGI_Base* GI = Cast<UGI_Base>(UGameplayStatics::GetGameInstance(this)))
	{
		const TArray<FCRelicData>& All = GI->GetAllRelicData();

		TArray<FCRelicData> Filtered;
		for (const auto& Relic : All)
		{
			if (!Relic.IsRecover) // 복원 유물 조건
				Filtered.Add(Relic);
		}

		InitRelicList(Filtered);
	}

	if (Overlay_Check) Overlay_Check->SetVisibility(ESlateVisibility::Hidden);
	if (Overlay_Reselect) Overlay_Reselect->SetVisibility(ESlateVisibility::Hidden);

	if (Btn_Prev) Btn_Prev->OnClicked.AddDynamic(this, &URestoreUI::OnPrevPage);
	if (Btn_Later) Btn_Later->OnClicked.AddDynamic(this, &URestoreUI::OnNextPage);

	if (Btn_Yes) Btn_Yes->OnClicked.AddDynamic(this, &URestoreUI::OnYes);
	if (Btn_No) Btn_No->OnClicked.AddDynamic(this, &URestoreUI::OnNo);
	if (Btn_Reselect) Btn_Reselect->OnClicked.AddDynamic(this, &URestoreUI::OnReselect);

	FindAndConnectRestoreManager();
}

void URestoreUI::FindAndConnectRestoreManager()
{
	for (TActorIterator<ARestoreManager> It(GetWorld()); It; ++It)
	{
		RestoreManager = *It;
		break;
	}

	if(RestoreManager) RestoreManager->SetRestoreUI(this);
}

void URestoreUI::InitRelicList(const TArray<FCRelicData>& RelicList)
{
	RestoreRelicList = RelicList;

	if (Txt_EmptyRelics)
	{
		const bool bEmpty = RelicList.Num() == 0;
		Txt_EmptyRelics->SetVisibility(bEmpty ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}

	CurrentPage = 0;
	MaxPage = FMath::CeilToInt((float)RelicList.Num() / (float)ItemsPerPage);

	if (RestoreRelicList.Num() > 0)
	{
		ShowPage(CurrentPage);
	}
	else
	{
		// 리스트가 없으면 기존 UI 모두 클리어
		//HorizontalBox_Row1->ClearChildren();
		//HorizontalBox_Row2->ClearChildren();
		//HorizontalBox_Row3->ClearChildren();
		
		// 기존 Overlay 클리어
		for (UOverlay* Overlay : RelicSlotOverlays)
		{
			if (Overlay) Overlay->ClearChildren();
		}

		if (Txt_Page) Txt_Page->SetText(FText::FromString(TEXT("0 / 0")));
	}
}

void URestoreUI::ShowPage(int32 PageIndex)
{
	// 1. 기존 항목 제거
	//HorizontalBox_Row1->ClearChildren();
	//HorizontalBox_Row2->ClearChildren();
	//HorizontalBox_Row3->ClearChildren();

	for (UOverlay* Overlay : RelicSlotOverlays)
	{
		if (Overlay) Overlay->ClearChildren();
	}

	// 2. 해당 페이지 유물 뽑기
	int32 StartIndex = PageIndex * ItemsPerPage;
	int32 EndIndex = FMath::Min(StartIndex + ItemsPerPage, RestoreRelicList.Num());

	// 3. 유물 배치
	for (int32 i = StartIndex; i < EndIndex; ++i)
	{
		int32 LocalIndex = i - StartIndex;

		if (RelicItemClass && RelicSlotOverlays.IsValidIndex(LocalIndex))
		{
			if (URestoreRelicUI* ItemWidget = CreateWidget<URestoreRelicUI>(this, RelicItemClass))
			{
				ItemWidget->InitItem(RestoreRelicList[i]);
				ItemWidget->OnRelicItemClicked.AddDynamic(this, &URestoreUI::HandleRelicItemClicked);

				UOverlay* TargetOverlay = RelicSlotOverlays[LocalIndex];
				if (TargetOverlay)
				{
					TargetOverlay->AddChild(ItemWidget);

					if (UOverlaySlot* AddedSlot = Cast<UOverlaySlot>(ItemWidget->Slot))
					{
						AddedSlot->SetHorizontalAlignment(HAlign_Fill);
						AddedSlot->SetVerticalAlignment(VAlign_Fill);
					}
				}
			}
		}
	}

	/*const int32 MaxPerRow = 3;

	for (int32 i = StartIndex; i < EndIndex; ++i)
	{
		int32 LocalIndex = i - StartIndex;
		int32 RowIndex = LocalIndex / MaxPerRow;

		UHorizontalBox* TargetRow = nullptr;
		switch (RowIndex)
		{
			case 0: TargetRow = HorizontalBox_Row1; break;
			case 1: TargetRow = HorizontalBox_Row2; break;
			case 2: TargetRow = HorizontalBox_Row3; break;
		}

		if (TargetRow && RelicItemClass)
		{
			if (URestoreRelicUI* ItemWidget = CreateWidget<URestoreRelicUI>(this, RelicItemClass))
			{
				ItemWidget->InitItem(RestoreRelicList[i]);
				ItemWidget->OnRelicItemClicked.AddDynamic(this, &URestoreUI::HandleRelicItemClicked);

				if (UHorizontalBoxSlot* AddedSlot = Cast<UHorizontalBoxSlot>(TargetRow->AddChild(ItemWidget)))
				{
					AddedSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
					AddedSlot->SetHorizontalAlignment(HAlign_Fill);
					AddedSlot->SetVerticalAlignment(VAlign_Fill);
				}
			}
		}
	}*/

	UpdatePage();
}

void URestoreUI::UpdatePage()
{
	if(Txt_Page) Txt_Page->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), CurrentPage + 1, MaxPage)));
}

void URestoreUI::OnNextPage()
{
	if (CurrentPage < MaxPage - 1)
	{
		++CurrentPage;
		ShowPage(CurrentPage);
	}
}

void URestoreUI::OnPrevPage()
{
	if (CurrentPage > 0)
	{
		--CurrentPage;
		ShowPage(CurrentPage);
	}
}

void URestoreUI::OnYes()
{
	if (RestoreManager)
	{
		RestoreManager->StartRestoration(SelectedRelicData);
		if (Overlay_Check) Overlay_Check->SetVisibility(ESlateVisibility::Hidden);
		if (Img_BG) Img_BG->SetVisibility(ESlateVisibility::Hidden);
		if (Overlay_List) Overlay_List->SetVisibility(ESlateVisibility::Hidden);
		if (Overlay_Reselect) Overlay_Reselect->SetVisibility(ESlateVisibility::Visible);

		UE_LOG(LogTemp, Warning, TEXT("Starting restoration for relic: %s"), *SelectedRelicData.RelicName.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("RestoreManager is null in RestoreUI"));
	}
}

void URestoreUI::OnNo()
{
	if (Overlay_Check) Overlay_Check->SetVisibility(ESlateVisibility::Hidden);
}

void URestoreUI::OnReselect()
{
	if (Overlay_Reselect) Overlay_Reselect->SetVisibility(ESlateVisibility::Hidden);

	if (Img_BG) Img_BG->SetVisibility(ESlateVisibility::Visible);
	if (Overlay_List) Overlay_List->SetVisibility(ESlateVisibility::Visible);
}

void URestoreUI::HandleRelicItemClicked(const FCRelicData& RelicData)
{
	SelectedRelicData = RelicData;

	if (Overlay_Check) Overlay_Check->SetVisibility(ESlateVisibility::Visible);
}
