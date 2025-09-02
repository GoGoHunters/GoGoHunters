// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LHJ/CRelicData.h"
#include "RestoreUI.generated.h"

/**
 * 
 */
UCLASS()
class GOGOHUNTERS_API URestoreUI : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

//public:
//	void PopulateRelicList(const TArray<FCRelicData>& RelicList);

protected:
	void FindAndConnectRestoreManager();

	void InitRelicList(const TArray<FCRelicData>& RelicList);
	void ShowPage(int32 PageIndex);
	void UpdatePage();

	UFUNCTION()
	void OnNextPage();
	UFUNCTION()
	void OnPrevPage();
	
	UFUNCTION()
	void OnYes();
	UFUNCTION()
	void OnNo();
	UFUNCTION()
	void OnReselect();

	UFUNCTION()
	void HandleRelicItemClicked(const FCRelicData& RelicData);

protected:
	/*UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* HorizontalBox_Row1;
	UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* HorizontalBox_Row2;
	UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* HorizontalBox_Row3;*/

	UPROPERTY(meta = (BindWidget)) class UOverlay* Overlay1_1;
	UPROPERTY(meta = (BindWidget)) class UOverlay* Overlay1_2;
	UPROPERTY(meta = (BindWidget)) class UOverlay* Overlay1_3;
	UPROPERTY(meta = (BindWidget)) class UOverlay* Overlay2_1;
	UPROPERTY(meta = (BindWidget)) class UOverlay* Overlay2_2;
	UPROPERTY(meta = (BindWidget)) class UOverlay* Overlay2_3;
	UPROPERTY(meta = (BindWidget)) class UOverlay* Overlay3_1;
	UPROPERTY(meta = (BindWidget)) class UOverlay* Overlay3_2;
	UPROPERTY(meta = (BindWidget)) class UOverlay* Overlay3_3;

	TArray<UOverlay*> RelicSlotOverlays;

	UPROPERTY(meta = (BindWidget))
	class UImage* Img_BG;

	UPROPERTY(meta = (BindWidget))
	class UOverlay* Overlay_List;

	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Prev;
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Later;

	UPROPERTY(meta = (BindeWidget))
	class UOverlay* Overlay_Check;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Yes;

	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_No;
	
	UPROPERTY(meta = (BindeWidget))
	class UOverlay* Overlay_Reselect;

	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Reselect;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Txt_Page;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Txt_EmptyRelics;

	UPROPERTY(EditDefaultsOnly)
	int32 ItemsPerPage = 9;
	int32 CurrentPage = 0;
	int32 MaxPage = 0;

	TArray<FCRelicData> RestoreRelicList;

	FCRelicData SelectedRelicData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RestoreUI")
	TSubclassOf<class URestoreRelicUI> RelicItemClass;

private:
	UPROPERTY()
	class ARestoreManager* RestoreManager;
};
