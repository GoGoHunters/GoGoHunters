// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_MessageUI.h"

#include "base/GI_Base.h"
#include "Components/Button.h"
#include "Components/GridSlot.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetSystemLibrary.h"


void UMH_MessageUI::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (Btn_Yes)Btn_Yes->OnClicked.AddDynamic(this, &UMH_MessageUI::OnYesClicked);
	if (Btn_No)Btn_No->OnClicked.AddDynamic(this, &UMH_MessageUI::OnNoClicked);
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
		if (TargetLevel == "Record")
		{
			//학주 //도감 위젯 띄우기
			GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,TEXT("Record"));
			OnNoClicked();
		}
		else if (UGI_Base* GameInstance = Cast<UGI_Base>(GetGameInstance()))
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

void UMH_MessageUI::OnNoClicked()
{
	OnCloseClicked.Broadcast();
}

void UMH_MessageUI::SetOuterActor(AMH_ZoneBase* InOwner)
{
	if (!InOwner) return;
	OuterOwner = InOwner;
}

void UMH_MessageUI::ShowButtons(bool bShowOkButton, bool bShowCloseButton)
{
	if (Btn_Yes)
	{
		Btn_Yes->SetVisibility(bShowOkButton ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
	if (Btn_No)
	{
		Btn_No->SetVisibility(bShowCloseButton ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
	// 버튼 가시성을 조절한 후, 필요에 따라 버튼 위치를 다시 정렬
	AlignButtons();
}

void UMH_MessageUI::AlignButtons()
{
	// 버튼 두개 중 하나만 Visible일 경우, 남은 버튼을 중앙으로 이동시킵니다.
	bool bYesVisible = Btn_Yes && Btn_Yes->GetVisibility() == ESlateVisibility::Visible;
	bool bNoVisible = Btn_No && Btn_No->GetVisibility() == ESlateVisibility::Visible;

	if (bYesVisible && !bNoVisible)
	{
		// OkButton만 보이는 경우, GridPanel 내에서 중앙 정렬
		if (UGridSlot* GridSlot = Cast<UGridSlot>(Btn_Yes->Slot))
		{
			GridSlot->SetColumn(0); // 첫 번째 컬럼으로 이동
			GridSlot->SetColumnSpan(1); // 두 컬럼에 걸쳐 중앙 정렬
			GridSlot->SetHorizontalAlignment(HAlign_Center);
		}
	}
	else if (!bYesVisible && bNoVisible)
	{
		// CloseButton만 보이는 경우, GridPanel 내에서 중앙 정렬
		if (UGridSlot* GridSlot = Cast<UGridSlot>(Btn_No->Slot))
		{
			GridSlot->SetColumn(0);
			GridSlot->SetColumnSpan(1);
			GridSlot->SetHorizontalAlignment(HAlign_Center);
		}
	}
	else if (bYesVisible && bNoVisible)
	{
		// 두 버튼 모두 보이는 경우, 원래 위치로 되돌립니다.
		if (UGridSlot* OkGridSlot = Cast<UGridSlot>(Btn_Yes->Slot))
		{
			OkGridSlot->SetColumn(0);
			OkGridSlot->SetColumnSpan(0);
			OkGridSlot->SetHorizontalAlignment(HAlign_Fill);
		}
		if (UGridSlot* CloseGridSlot = Cast<UGridSlot>(Btn_No->Slot))
		{
			CloseGridSlot->SetColumn(2);
			CloseGridSlot->SetColumnSpan(0);
			CloseGridSlot->SetHorizontalAlignment(HAlign_Fill);
		}
	}
}
