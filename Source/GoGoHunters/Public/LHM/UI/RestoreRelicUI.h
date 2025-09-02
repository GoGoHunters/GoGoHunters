// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LHJ/CRelicData.h"
#include "RestoreRelicUI.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRelicItemClicked, const FCRelicData&, RelicData);

/**
 * 
 */
UCLASS()
class GOGOHUNTERS_API URestoreRelicUI : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

public:
	void InitItem(const FCRelicData& InData);

	UPROPERTY(BlueprintAssignable)
	FOnRelicItemClicked OnRelicItemClicked;

protected:
	UFUNCTION()
	void HandleButtonClicked();

protected:
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Relic;

	UPROPERTY(EditDefaultsOnly, meta=(BindWidget))
	class UImage* Img_Relic;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Txt_RelicName;

private:
	FCRelicData RelicData;
};
