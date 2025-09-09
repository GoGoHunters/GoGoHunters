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

	UFUNCTION(BlueprintCallable)
	void SetVisibilityFlagTrigger(bool bVisible);

	UFUNCTION(BlueprintCallable)
	void SetVisibilityLobby(bool bVisible);

	UFUNCTION(BlueprintCallable)
	void SetVisibilityRestore(bool bVisible);

protected:
	UFUNCTION()
	void OnClick_FlagTrigger();
	
	UFUNCTION()
	void OnClick_Lobby();
	
	UFUNCTION()
	void OnClick_Restore();

	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_FlagTrigger;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Lobby;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Restore;

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

public:
	UPROPERTY(BlueprintReadWrite)
	bool bUseFlagTrigger = false; // Flag Trigger 버튼 사용 여부
};
