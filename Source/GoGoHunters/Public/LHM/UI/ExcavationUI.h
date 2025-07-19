// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ExcavationUI.generated.h"

/**
 * 
 */
UCLASS()
class GOGOHUNTERS_API UExcavationUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

protected:
	// 버튼 위젯
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Tool1;

	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Tool2;

	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Tool3;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Tool4;

	// 바인딩 함수
	UFUNCTION()
	void OnClick_DetectionTool();

	UFUNCTION()
	void OnClick_ShovelTool();

	UFUNCTION()
	void OnClick_BrushTool();
	
	UFUNCTION()
	void OnClick_TweezerTool();

private:
	UPROPERTY()
	class AMH_VRPlayer* VRPlayer;
};
