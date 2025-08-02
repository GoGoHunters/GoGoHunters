// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DetectionUI.generated.h"

/**
 * 
 */
UCLASS()
class GOGOHUNTERS_API UDetectionUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
    UFUNCTION(BlueprintCallable)
    void UpdateUI(float Progress, bool bIsStop);

private:
    UPROPERTY(meta = (BindWidget))
    class UProgressBar* DetectionProgressBar;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* StatusText;

    void UpdateStatusText(float Progress, bool bIsStop);
};
