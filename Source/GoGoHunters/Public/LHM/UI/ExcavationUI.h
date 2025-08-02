// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LHM/Excavation/ExcavationManager.h"
#include "UIs/CWidgetBase.h"
#include "ExcavationUI.generated.h"

/**
 * 
 */
UCLASS()
class GOGOHUNTERS_API UExcavationUI : public UCWidgetBase
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// 특정 도구 버튼 활성화/비활성화
	UFUNCTION(BlueprintCallable, Category = "Excavation UI")
	void SetToolButtonEnabled(int32 ToolIndex, bool bEnabled);

	// 발굴 단계 변경 콜백
	UFUNCTION()
	void OnExcavationPhaseChanged(EExcavationPhase NewPhase);

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
	// ExcavationManager 찾기 및 연결
	void FindAndConnectExcavationManager();

	UPROPERTY()
	class AMH_VRPlayer* VRPlayer;

	UPROPERTY()
	class AExcavationManager* ExcavationManager;

	// 현재 발굴 단계
	UPROPERTY()
	EExcavationPhase CurrentPhase = EExcavationPhase::Detection;
};
