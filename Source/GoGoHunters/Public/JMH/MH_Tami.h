// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MH_Tami.generated.h"

/**
 * 
 */
UCLASS()
class GOGOHUNTERS_API UMH_Tami : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UButton> Btn_Audio;
	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UButton> Btn_TamiExit;

	UFUNCTION()
	void Btn_AudioPressed();
	UFUNCTION()
	void Btn_TamiExitClicked();
	
};
