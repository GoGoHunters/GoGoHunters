// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ExcavationProgressUI.generated.h"

enum class EExcavationPhase : uint8;

/**
 * 
 */
UCLASS()
class GOGOHUNTERS_API UExcavationProgressUI : public UUserWidget
{
	GENERATED_BODY()
	

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:

	// 발굴 단계 변경 콜백
	UFUNCTION()
	void OnExcavationPhaseChanged(EExcavationPhase NewPhase);
	void UpdatePhaseImagesOpacity(int32 ActiveIndex);

	UPROPERTY(meta = (BindWidget))
	class UImage* Img_Phase1;
	UPROPERTY(meta = (BindWidget))
	class UImage* Img_Phase2;
	UPROPERTY(meta = (BindWidget))
	class UImage* Img_Phase3;
	UPROPERTY(meta = (BindWidget))
	class UImage* Img_Phase4;

protected:
	UPROPERTY(meta = (BindWidget))
    class UDiggingUI* DiggingUI;
	
    UPROPERTY(meta = (BindWidget))
    class UBrushingUI* BrushingUI;

private:
	void FindAndConnectExcavationManager();

	UPROPERTY()
	class AExcavationManager* ExcavationManager;

public:
	void SetOwningWidgetActor(class AExcavationProgressWidgetActor* InActor) { OwningWidgetActor = InActor; }

private:
	UPROPERTY()
	class AExcavationProgressWidgetActor* OwningWidgetActor;

	int32 HighestPhaseIndexReached = -1;
};
