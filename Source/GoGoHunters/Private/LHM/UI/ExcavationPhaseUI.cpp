// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/UI/ExcavationPhaseUI.h"
#include "Components/Button.h"
#include "LHM/Excavation/ExcavationManager.h"
#include "EngineUtils.h"
#include "LHM/Excavation/ExcavationWidgetActor.h"
#include "base/GI_Base.h"

void UExcavationPhaseUI::NativeConstruct()
{
	FindAndConnectExcavationManager();

	if (Btn_FlagTrigger)
	{
		Btn_FlagTrigger->OnClicked.AddDynamic(this, &UExcavationPhaseUI::OnClick_FlagTrigger);
		Btn_FlagTrigger->SetVisibility(ESlateVisibility::Hidden);
	}

	if (Btn_Lobby)
	{
		Btn_Lobby->OnClicked.AddDynamic(this, &UExcavationPhaseUI::OnClick_Lobby);
		Btn_Lobby->SetVisibility(ESlateVisibility::Hidden);
	}

	if (Btn_Restore)
	{
		Btn_Restore->OnClicked.AddDynamic(this, &UExcavationPhaseUI::OnClick_Restore);
		Btn_Restore->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UExcavationPhaseUI::SetVisibilityFlagTrigger(bool bVisible)
{
	if (!Btn_FlagTrigger || !OwningWidgetActor) return;
	
	Btn_FlagTrigger->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	OwningWidgetActor->SetCollisionAndVisible(bVisible);
}

void UExcavationPhaseUI::SetVisibilityLobby(bool bVisible)
{
	if (!Btn_Lobby || !OwningWidgetActor) return;

	Btn_Lobby->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	OwningWidgetActor->SetCollisionAndVisible(bVisible);
}

void UExcavationPhaseUI::SetVisibilityRestore(bool bVisible)
{
	if (!Btn_Restore || !OwningWidgetActor) return;

	Btn_Restore->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	OwningWidgetActor->SetCollisionAndVisible(bVisible);
}

void UExcavationPhaseUI::OnClick_FlagTrigger()
{
	if (ExcavationManager) ExcavationManager->ChangeExcavationPhase();
}

void UExcavationPhaseUI::OnClick_Lobby()
{
	if (UGI_Base* GI = Cast<UGI_Base>(GetGameInstance()))
	{
		GI->TransitionToLevel(FString("LV_TestLobby"));
	}
}

void UExcavationPhaseUI::OnClick_Restore()
{
	if (UGI_Base* GI = Cast<UGI_Base>(GetGameInstance()))
	{
		GI->TransitionToLevel(FString("LV_Restore"));
	}
}

void UExcavationPhaseUI::FindAndConnectExcavationManager()
{
	for (TActorIterator<AExcavationManager> It(GetWorld()); It; ++It)
	{
		ExcavationManager = *It;
		break;
	}

	if(ExcavationManager) ExcavationManager->SetPhaseUI(this);
}