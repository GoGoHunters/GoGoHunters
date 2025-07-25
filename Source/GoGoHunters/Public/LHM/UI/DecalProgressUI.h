// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DecalProgressUI.generated.h"

/**
 * 
 */
UCLASS()
class GOGOHUNTERS_API UDecalProgressUI : public UUserWidget
{
	GENERATED_BODY()

public:
    UPROPERTY(meta = (BindWidget))
    class UProgressBar* ProgressBar;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* Txt_Percent;

    void UpdateProgress(float Opacity);
    void SetProgressBarImage(int32 Index);
};
