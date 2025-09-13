// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/UI/WarningTextUI.h"
#include "Components/Overlay.h"
#include "LHM/Excavation/ExcavationManager.h"
#include "EngineUtils.h"

void UWarningTextUI::NativeConstruct()
{
	FindAndConnectExcavationManager();
	//Overlay->SetVisibility(ESlateVisibility::Hidden);
}

void UWarningTextUI::SetVisibilityOverlay(bool bVisible)
{
	Overlay->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

void UWarningTextUI::PlayAlertAnim()
{
	PlayAnimation(Alert, 0, 1, EUMGSequencePlayMode::Forward, 1, true);
}

void UWarningTextUI::FindAndConnectExcavationManager()
{
	for (TActorIterator<AExcavationManager> It(GetWorld()); It; ++It)
	{
		(*It)->SetWarningTextUI(this);
		break;
	}
}