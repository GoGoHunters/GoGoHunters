// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ExcavationPhaseUI.generated.h"

/**
 * 
 */
UCLASS()
class GOGOHUNTERS_API UExcavationPhaseUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	void SetVisibilityFlagTrigger(bool bVisible);
	void SetVisibilityCloseLid(bool bVisible);
	void SetVisibilityLobby(bool bVisible);
	void SetVisibilityMuseum(bool bVisible);

protected:
	UFUNCTION()
	void OnClick_FlagTrigger();

	UFUNCTION()
	void OnClick_CloseLid();
	
	UFUNCTION()
	void OnClick_Lobby();
	
	UFUNCTION()
	void OnClick_Museum();

	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_FlagTrigger;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_CloseLid;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Lobby;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Museum;

private:
	// ExcavationManager 찾기 및 연결
	void FindAndConnectExcavationManager();

	UPROPERTY()
	class AExcavationManager* ExcavationManager;

public:
	void SetOwningWidgetActor(class AExcavationWidgetActor* InActor) { OwningWidgetActor = InActor; }

private:
	UPROPERTY()
	class AExcavationWidgetActor* OwningWidgetActor;
};
