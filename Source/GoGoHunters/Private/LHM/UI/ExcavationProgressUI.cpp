// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/UI/ExcavationProgressUI.h"
#include "LHM/Excavation/ExcavationManager.h"
#include "EngineUtils.h"
#include "LHM/UI/DiggingUI.h"
#include "LHM/UI/BrushingUI.h"

void UExcavationProgressUI::NativeConstruct()
{
	FindAndConnectExcavationManager();

	if (DiggingUI) DiggingUI->SetVisibility(ESlateVisibility::Hidden);
	if (BrushingUI) BrushingUI->SetVisibility(ESlateVisibility::Hidden);
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
