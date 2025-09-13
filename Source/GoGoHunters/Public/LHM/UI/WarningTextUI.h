// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WarningTextUI.generated.h"

/**
 * 
 */
UCLASS()
class GOGOHUNTERS_API UWarningTextUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	void SetVisibilityOverlay(bool bVisible);

	void PlayAlertAnim();

private:
	UPROPERTY(meta = (BindWidget))
	class UOverlay* Overlay;
	
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* Alert;

	void FindAndConnectExcavationManager();
};
