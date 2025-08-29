// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WarningUI.generated.h"

/**
 * 
 */
UCLASS()
class GOGOHUNTERS_API UWarningUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	UFUNCTION()
    void ShowNextWarning();

private:
	UPROPERTY(meta = (BindWidget))
	class UOverlay* Overlay_Warning;
	
	UPROPERTY(meta = (BindWidget))
	class UOverlay* Overlay_Failure;

	UPROPERTY(meta = (BindWidget))
    class UTextBlock* Txt_WarningMessage;
    
	UPROPERTY(meta = (BindWidget))
	class UImage* Img_WarningIcon1;
	UPROPERTY(meta = (BindWidget))
	class UImage* Img_WarningIcon2;
	UPROPERTY(meta = (BindWidget))
	class UImage* Img_WarningIcon3;

	TArray<class UImage*> WarningIcons;

	int32 CurrentWarningCount = 0;

public:
	void SetOwningWidgetActor(class AExcavationWarningWidgetActor* InActor) { OwningWidgetActor = InActor; }

	void SetWarningVisibility(bool bVisible);

	void ResetWarnings();

private:
	UPROPERTY()
	class AExcavationWarningWidgetActor* OwningWidgetActor;

	void FindAndConnectExcavationManager();

	UPROPERTY()
	class AExcavationManager* ExcavationManager;
};
