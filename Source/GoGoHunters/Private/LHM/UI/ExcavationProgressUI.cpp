// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/UI/ExcavationProgressUI.h"
#include "LHM/Excavation/ExcavationManager.h"
#include "EngineUtils.h"
#include "LHM/UI/DiggingUI.h"
#include "LHM/UI/BrushingUI.h"
#include "Components/Image.h"

void UExcavationProgressUI::NativeConstruct()
{
	FindAndConnectExcavationManager();

	// 델리게이트 구독 및 초기 이미지 상태 반영
	if (ExcavationManager)
	{
		ExcavationManager->OnExcavationPhaseChanged.AddDynamic(this, &UExcavationProgressUI::OnExcavationPhaseChanged);
		OnExcavationPhaseChanged(ExcavationManager->GetCurrentPhase());
	}

	if (DiggingUI) DiggingUI->SetVisibility(ESlateVisibility::Hidden);
	if (BrushingUI) BrushingUI->SetVisibility(ESlateVisibility::Hidden);
}

void UExcavationProgressUI::NativeDestruct()
{
	if (ExcavationManager)
	{
		ExcavationManager->OnExcavationPhaseChanged.RemoveDynamic(this, &UExcavationProgressUI::OnExcavationPhaseChanged);
	}
	Super::NativeDestruct();
}

void UExcavationProgressUI::OnExcavationPhaseChanged(EExcavationPhase NewPhase)
{
	// EExcavationPhase: Detection(0), Digging(1), Brushing(2), Collection(3), Completed(4)
	int32 ActiveIndex = -1;
	switch (NewPhase)
	{
		case EExcavationPhase::Detection: ActiveIndex = 0; break;
		case EExcavationPhase::Digging: ActiveIndex = 1; break;
		case EExcavationPhase::Brushing: ActiveIndex = 2; break;
		case EExcavationPhase::Collection: ActiveIndex = 3; break;
		case EExcavationPhase::Completed: ActiveIndex = 3; break;
		default: break;
	}

	// 최고 도달 단계 갱신 (Completed는 3으로 고정 유지)
	if (ActiveIndex >= 0)
	{
		HighestPhaseIndexReached = FMath::Max(HighestPhaseIndexReached, ActiveIndex);
	}

	UpdatePhaseImagesOpacity(ActiveIndex);
}

void UExcavationProgressUI::UpdatePhaseImagesOpacity(int32 ActiveIndex)
{
	const float ActiveOpacity = 1.0f;
	const float InactiveOpacity = 0.3f;

	auto SetOpacityIfValid = [](UImage* Image, float Opacity)
		{
			if (Image)
			{
				Image->SetRenderOpacity(Opacity);
			}
		};
		 
	// 지나간(최고 도달 이하) 단계는 항상 1.0 유지
	SetOpacityIfValid(Img_Phase1, 0 <= HighestPhaseIndexReached ? ActiveOpacity : (ActiveIndex == 0 ? ActiveOpacity : InactiveOpacity));
	SetOpacityIfValid(Img_Phase2, 1 <= HighestPhaseIndexReached ? ActiveOpacity : (ActiveIndex == 1 ? ActiveOpacity : InactiveOpacity));
	SetOpacityIfValid(Img_Phase3, 2 <= HighestPhaseIndexReached ? ActiveOpacity : (ActiveIndex == 2 ? ActiveOpacity : InactiveOpacity));
	SetOpacityIfValid(Img_Phase4, 3 <= HighestPhaseIndexReached ? ActiveOpacity : (ActiveIndex == 3 ? ActiveOpacity : InactiveOpacity));
}

void UExcavationProgressUI::FindAndConnectExcavationManager()
{
	for (TActorIterator<AExcavationManager> It(GetWorld()); It; ++It)
	{
		ExcavationManager = *It;
		break;
	}

	if (ExcavationManager)
	{
		if (DiggingUI) ExcavationManager->SetDiggingUI(DiggingUI);
		if (BrushingUI) ExcavationManager->SetBrushingUI(BrushingUI);
	}
}
