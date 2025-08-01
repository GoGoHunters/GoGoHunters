// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_MessageUI.h"

#include "base/GI_Base.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetSystemLibrary.h"


void UMH_MessageUI::NativeConstruct()
{
	Super::NativeConstruct();


	if (Btn_Yes)
	{
		Btn_Yes->OnClicked.AddDynamic(this, &UMH_MessageUI::OnYesClicked);
	}
}

void UMH_MessageUI::SetMessage(const FText& NewMessage)
{
	if (Text_Message)
	{
		Text_Message->SetText(NewMessage);
	}
}

void UMH_MessageUI::OnYesClicked()
{
	if (TargetLevel != "Exit")
	{
		if (UGI_Base* GameInstance = Cast<UGI_Base>(GetGameInstance()))
		{
			GameInstance->TransitionToLevel(TargetLevel);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Target level name is empty for continent: %s"), *TargetLevel);
		}
	}
	else if (TargetLevel == "Exit")
	{
		UKismetSystemLibrary::QuitGame(this, GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit, false);
	}
}

void UMH_MessageUI::SetOuterActor(AMH_ZoneBase* InOwner)
{
	if (!InOwner) return;
	OuterOwner = InOwner;
}
