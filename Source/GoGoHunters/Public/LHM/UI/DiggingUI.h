// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DiggingUI.generated.h"

/**
 * 
 */
UCLASS()
class GOGOHUNTERS_API UDiggingUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
    UFUNCTION(BlueprintCallable)
    void UpdateUI(float Progress);

private:
    UPROPERTY(meta = (BindWidget))
    class UProgressBar* DiggingProgressBar;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* StatusText;

    void UpdateStatusText(float Progress);
};
