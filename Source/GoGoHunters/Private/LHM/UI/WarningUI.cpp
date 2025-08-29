// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/UI/WarningUI.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "LHM/Excavation/ExcavationWarningWidgetActor.h"
#include "LHM/Excavation/ExcavationManager.h"
#include "EngineUtils.h"
#include "Components/WidgetComponent.h"

void UWarningUI::NativeConstruct()
{
	WarningIcons = 
	{
		Img_WarningIcon1,
		Img_WarningIcon2,
		Img_WarningIcon3
	};

	for(UImage* Icon : WarningIcons)
	{
		if (Icon) Icon->SetVisibility(ESlateVisibility::Hidden);
	}

	Overlay_Failure->SetVisibility(ESlateVisibility::Hidden);

	FindAndConnectExcavationManager();
}
  
void UWarningUI::ShowNextWarning()
{
	CurrentWarningCount++;

	if (CurrentWarningCount == 1)
	{
		SetWarningVisibility(true);
	}

	// 1~3 아이콘 점등
	if (CurrentWarningCount <= WarningIcons.Num())
	{
		if (CurrentWarningCount == WarningIcons.Num())
		{
			// 3회: 실패
			if (Overlay_Warning) Overlay_Warning->SetVisibility(ESlateVisibility::Hidden);
			if (Overlay_Failure) Overlay_Failure->SetVisibility(ESlateVisibility::Visible);

			// 3초 뒤 복구
			FTimerHandle Handle;
			if (UWorld* World = GetWorld())
			{
				World->GetTimerManager().SetTimer(Handle, [this]()
				{
					if (ExcavationManager) ExcavationManager->HandleWarningReset();
					ResetWarnings();

				}, 3.0f, false);
			}
			return;
		}

		if (WarningIcons.IsValidIndex(CurrentWarningCount - 1) && WarningIcons[CurrentWarningCount - 1])
		{
			WarningIcons[CurrentWarningCount - 1]->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void UWarningUI::SetWarningVisibility(bool bVisible)
{
	if (OwningWidgetActor && OwningWidgetActor->GetWidgetComponent())
	{
		OwningWidgetActor->GetWidgetComponent()->SetHiddenInGame(!bVisible);
	}
}

void UWarningUI::ResetWarnings()
{
	// 아이콘 숨기기
	for (UImage* Icon : WarningIcons)
	{
		if (Icon) Icon->SetVisibility(ESlateVisibility::Hidden);
	}

	// 실패 오버레이 숨기기, 일반 경고 복구
	if (Overlay_Failure) Overlay_Failure->SetVisibility(ESlateVisibility::Hidden);
	if (Overlay_Warning) Overlay_Warning->SetVisibility(ESlateVisibility::Visible);

	SetWarningVisibility(false);

	CurrentWarningCount = 0;
}

void UWarningUI::FindAndConnectExcavationManager()
{
	for (TActorIterator<AExcavationManager> It(GetWorld()); It; ++It)
	{
		ExcavationManager = *It;
		break;
	}

	if (ExcavationManager) ExcavationManager->SetWarningUI(this);
}
