#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LHJ/CRelicData.h"
#include "CRelicWidget.generated.h"

class UGI_Base;
class UButton;
class UImage;
class UTextBlock;

/**
 * 
 */
UCLASS()
class GOGOHUNTERS_API UCRelicWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetRelicData(const FCRelicData& InRelicData) { RelicData = InRelicData; }

private:
	UPROPERTY(EditDefaultsOnly, meta=(BindWidget))
	TObjectPtr<UTextBlock> Txt_RelicName;
	UPROPERTY(EditDefaultsOnly, meta=(BindWidget))
	TObjectPtr<UImage> Img_Relic;
	UPROPERTY(EditDefaultsOnly, meta=(BindWidget))
	TObjectPtr<UButton> Btn_SelectRelic;

	FCRelicData RelicData;
	const FCRelicDetailData* RelicDetailData = nullptr;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UGI_Base> GI;

	virtual void NativeConstruct() override;
	void SetRelicDetailData();
	UFUNCTION()
	void OnSelectRelicButtonClicked();
};
