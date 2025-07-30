// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ExcavationProgressUI.generated.h"

/**
 * 
 */
UCLASS()
class GOGOHUNTERS_API UExcavationProgressUI : public UUserWidget
{
	GENERATED_BODY()
	

public:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(meta = (BindWidget))
    class UDiggingUI* DiggingUI;
	
    UPROPERTY(meta = (BindWidget))
    class UBrushingUI* BrushingUI;

private:
	void FindAndConnectExcavationManager();

	UPROPERTY()
	class AExcavationManager* ExcavationManager;
};
