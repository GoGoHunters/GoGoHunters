// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/UI/RestorationCompleteUI.h"
#include "Components/Button.h"
#include "base/GI_Base.h"
#include "LHM/Restore/RestorWidgetActor.h"
#include "EngineUtils.h"
#include "LHM/Restore/RestoreManager.h"

void URestorationCompleteUI::NativeConstruct()
{
	FindAndConnectRestoreManager();

	if (Btn_Lobby)
	{
		Btn_Lobby->OnClicked.AddDynamic(this, &URestorationCompleteUI::OnClick_Lobby);
	}

	if (Btn_Museum)
	{
		Btn_Museum->OnClicked.AddDynamic(this, &URestorationCompleteUI::OnClick_Museum);
	}
}

void URestorationCompleteUI::SetCompleteVisibility(bool bVisible)
{
	if (OwningWidgetActor) OwningWidgetActor->ShowCompleteUI(bVisible);
}

void URestorationCompleteUI::OnClick_Lobby()
{
	if (UGI_Base* GI = Cast<UGI_Base>(GetGameInstance()))
	{
		GI->TransitionToLevel(FString("LV_TestLobby"));
	}
}

void URestorationCompleteUI::OnClick_Museum()
{
	if (UGI_Base* GI = Cast<UGI_Base>(GetGameInstance()))
	{
		GI->TransitionToLevel(FString("LV_MyMuseum"));
	}
}

void URestorationCompleteUI::FindAndConnectRestoreManager()
{
	for (TActorIterator<ARestoreManager> It(GetWorld()); It; ++It)
	{
		RestoreManager = *It;
		break;
	}

	RestoreManager->SetCompleteUI(this);
}
