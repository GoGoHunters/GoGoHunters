#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CRelicCollectionWidget.generated.h"

struct FCRelicData;
class UCRelicWidget;
class UButton;
class UTextBlock;
class UGridPanel;
class UGI_Base;
/**
 * 
 */
UCLASS()
class GOGOHUNTERS_API UCRelicCollectionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	bool InitRelicWidgets();
	
private:
	UPROPERTY(EditDefaultsOnly, meta=(BindWidget))
	TObjectPtr<UGridPanel> Grid_Relics;
	UPROPERTY(EditDefaultsOnly, meta=(BindWidget))
	TObjectPtr<UTextBlock> Txt_EmptyRelics;
	UPROPERTY(EditDefaultsOnly, meta=(BindWidget))
	TObjectPtr<UTextBlock> Txt_Page;
	UPROPERTY(EditDefaultsOnly, meta=(BindWidget))
	TObjectPtr<UButton> Btn_Prev;
	UPROPERTY(EditDefaultsOnly, meta=(BindWidget))
	TObjectPtr<UButton> Btn_Later;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UGI_Base> GI;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UCRelicWidget> RelicWidgetFactory;

	int32 ItemsPerPage = 4;
	int32 CurrentPage = 0;
	int32 MaxPage = 0;
	TArray<FCRelicData> PlaceableRelics;
	
	virtual void NativeConstruct() override;
	void ShowPage(int32 PageIndex);
	void UpdatePage();
	UFUNCTION()
	void OnPrevPage();
	UFUNCTION()
	void OnNextPage();
};
