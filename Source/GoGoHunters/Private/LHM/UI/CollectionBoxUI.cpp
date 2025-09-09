// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/UI/CollectionBoxUI.h"
#include "Components/Button.h"
#include "LHM/Excavation/ExcavationManager.h"
#include "EngineUtils.h"
#include "LHM/Excavation/CollectionBox.h"

void UCollectionBoxUI::NativeConstruct()
{
	FindAndConnectExcavationManager();

	if (Btn_CloseLid)
	{
		Btn_CloseLid->OnClicked.AddDynamic(this, &UCollectionBoxUI::OnClick_CloseLid);
		Btn_CloseLid->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UCollectionBoxUI::SetVisibilityCloseLid(bool bVisible)
{
	if (!Btn_CloseLid || !OwningWidgetActor) return;

	Btn_CloseLid->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	OwningWidgetActor->SetColleionCloseBtnUI(bVisible ? true : false);
}

void UCollectionBoxUI::OnClick_CloseLid()
{
	if (!ExcavationManager) return;

	ExcavationManager->ChangeCompletedPhase();
}

void UCollectionBoxUI::FindAndConnectExcavationManager()
{
	for (TActorIterator<AExcavationManager> It(GetWorld()); It; ++It)
	{
		ExcavationManager = *It;
		break;
	}

	if (ExcavationManager) ExcavationManager->SetCollectionBoxUI(this);
}
