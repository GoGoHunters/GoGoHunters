// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "JMH/MH_ZoneBase.h"
#include "Blueprint/UserWidget.h"
#include "MH_MessageUI.generated.h"

/**
 * 
 */

UCLASS()
class GOGOHUNTERS_API UMH_MessageUI : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;

public:
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	class UButton* Btn_Yes;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	class UTextBlock* Text_Message;

	// 레벨 이동 요청시 전달할 맵 이름
	UPROPERTY()
	FString TargetLevel;
	
	UFUNCTION()
	void SetMessage(const FText& NewMessage);

	UFUNCTION()
	void OnYesClicked();
	
	UFUNCTION()
	void SetOuterActor(AMH_ZoneBase* InOwner);

	UPROPERTY()
	TObjectPtr<AMH_ZoneBase> OuterOwner;
	
};
