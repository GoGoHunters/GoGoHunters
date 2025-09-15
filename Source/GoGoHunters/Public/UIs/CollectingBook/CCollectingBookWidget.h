#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LHJ/CRelicData.h"
#include "CCollectingBookWidget.generated.h"

class UImage;
class UTextBlock;
class UUniformGridPanel;
class UCCollectingItem;
class UButton;
class UWidgetSwitcher;
struct FCRelicCollectingBook;
class UGI_Base;
/**
 * 
 */
UCLASS()
class GOGOHUNTERS_API UCCollectingBookWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void InitData();
	void ShowRelicDetailPage(FCRelicCollectingBook& InRelicCollectingBookData) const;
	
private:
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> WS_Main;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UButton> Btn_Prev;
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UButton> Btn_Later;
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UUniformGridPanel> Grid_Relics;
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> Txt_Page;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UImage> Img_Thumbnail;
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UButton> Btn_Back;
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> Txt_RelicName;
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> Txt_Continent;
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> Txt_DropDate;
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> Txt_Desc;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UImage> Img_BlindThumbnail;
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UButton> Btn_Back_1;
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> Txt_HiddenDesc;
	
	UPROPERTY()
	TObjectPtr<UGI_Base> GI;
	UPROPERTY()
	TArray<FCRelicCollectingBook> CollectingData;
	UPROPERTY(EditDefaultsOnly)
	int32 ItemsPerPage = 12;
	UPROPERTY(EditDefaultsOnly)
	int32 Columns = 3;

	int32 CurrentPage = 0;
	int32 MaxPage = 0;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UCCollectingItem> CollectingItemWidgetFactory;

	virtual void NativeConstruct() override;
	void ShowPage(int32 PageIndex);
	void UpdatePage();

	UFUNCTION()
	void GoToList();
	UFUNCTION()
	void OnPrevPage();
	UFUNCTION()
	void OnNextPage();
};
