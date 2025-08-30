// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "JMH/MH_ZoneBase.h"
#include "Blueprint/UserWidget.h"
#include "MH_MessageUI.generated.h"

class ACCollectingBook;
/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCloseClicked);

UCLASS()
class GOGOHUNTERS_API UMH_MessageUI : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;

public:
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnCloseClicked OnCloseClicked;
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	class UButton* Btn_Yes;
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	class UButton* Btn_No;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	class UGridPanel* Grid_Btn;

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
	void OnNoClicked();
	UFUNCTION()
	void SetOuterActor(AMH_ZoneBase* InOwner);
	
	// 외부에서 버튼 표시 설정을 할 Public 함수
	void ShowButtons(bool bShowOkButton, bool bShowCloseButton);

	// GridPanel에 버튼이 하나만 남았을 때 중앙 정렬을 위한 함수 (선택 사항)
	void AlignButtons();

	UPROPERTY()
	TObjectPtr<AMH_ZoneBase> OuterOwner;
	UPROPERTY()
	TObjectPtr<ACCollectingBook> CollectingBook;
};
