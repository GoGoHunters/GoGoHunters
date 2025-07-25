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
    virtual void NativeConstruct() override;

    UPROPERTY(meta = (BindWidget))
    class UProgressBar* ProgressBar;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* Txt_Percent;

	UPROPERTY(meta = (BindWidget))
	class UImage* Img_Collected;

    void UpdateProgress(float Opacity);
    void SetProgressBarImage(int32 Index);
    void SetCollectedImage(bool bCollected);
};
