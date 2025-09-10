// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RestorationCompleteUI.generated.h"

/**
 * 
 */
UCLASS()
class GOGOHUNTERS_API URestorationCompleteUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	void SetOwningWidgetActor(class ARestorWidgetActor* InActor) { OwningWidgetActor = InActor; }

	void SetCompleteVisibility(bool bVisible);

protected:
	UFUNCTION()
	void OnClick_Lobby();
	
	UFUNCTION()
	void OnClick_Museum();

	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Lobby;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Museum;

private:
	void FindAndConnectRestoreManager();

	UPROPERTY()
	class ARestoreManager* RestoreManager;
	
	UPROPERTY()
	class ARestorWidgetActor* OwningWidgetActor;
};
