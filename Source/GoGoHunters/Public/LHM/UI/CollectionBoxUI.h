// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CollectionBoxUI.generated.h"

/**
 * 
 */
UCLASS()
class GOGOHUNTERS_API UCollectionBoxUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	void SetVisibilityCloseLid(bool bVisible);

protected:
	UFUNCTION()
	void OnClick_CloseLid();

	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_CloseLid;

private:
	// ExcavationManager 찾기 및 연결
	void FindAndConnectExcavationManager();
	
	UPROPERTY()
	class AExcavationManager* ExcavationManager;

public:
	void SetOwningWidgetActor(class ACollectionBox* InActor) { OwningWidgetActor = InActor; }

private:
	UPROPERTY()
	class ACollectionBox* OwningWidgetActor;
};
