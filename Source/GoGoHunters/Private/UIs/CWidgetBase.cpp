#include "UIs/CWidgetBase.h"

#include "base/GI_Base.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "JMH/CMuseumComponent.h"
#include "UIs/CUiActor.h"

class UGI_Base;
class UCMuseumComponent;

void UCWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();
	if (Btn_A)
		Btn_A->OnClicked.AddDynamic(this, &UCWidgetBase::OnBtnAClicked);
	if (Btn_B)
		Btn_B->OnClicked.AddDynamic(this, &UCWidgetBase::OnBtnBClicked);
	if (Btn_Tami)
		Btn_Tami->OnClicked.AddDynamic(this, &UCWidgetBase::OnBtnTamiClicked);
	if (Btn_Lobby)
		Btn_Lobby->OnClicked.AddDynamic(this, &UCWidgetBase::OnBtnLobbyClicked);
	if (Btn_Settings)
		Btn_Settings->OnClicked.AddDynamic(this, &UCWidgetBase::OnBtnSetClicked);
	if (Btn_Exit)
		Btn_Exit->OnClicked.AddDynamic(this, &UCWidgetBase::OnBtnExitClicked);
}

void UCWidgetBase::OnBtnAClicked()
{
	SwitchUI(0);
}

void UCWidgetBase::OnBtnBClicked()
{
	SwitchUI(1);
}

void UCWidgetBase::SwitchUI(const int32 InDesiredUiIdx)
{
	if (!UiActor) return;
	UiActor->SwitchActiveWidget(InDesiredUiIdx);
}

void UCWidgetBase::BlindActive(bool bUseBlind)
{
	if (bUseBlind)
		Img_Blind->SetVisibility(ESlateVisibility::Visible);
	else
		Img_Blind->SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UCWidgetBase::K2_PlayUiAnim_Implementation(bool bIsReverse)
{
}


void UCWidgetBase::PlaySystemMessageAnim()
{
	GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,TEXT("Anim_ShowMessage11"));
	if (Anim_ShowMessage)
	{
		GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,TEXT("Anim_ShowMessage22"));
		PlayAnimation(Anim_ShowMessage,0.0f,1,EUMGSequencePlayMode::Forward, 1.0f, false);
	}
}

void UCWidgetBase::OnBtnTamiClicked()
{
	GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,TEXT("타미 나오세여"));
}

void UCWidgetBase::OnBtnLobbyClicked()
{
	// GameInstance를 통해 레벨 전환
	if (UGI_Base* GameInstance = Cast<UGI_Base>(GetGameInstance()))
	{
		FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(GetWorld(), true);
		// 현재 대륙의 타겟 레벨로 이동
		if (CurrentLevelName != "LV_TestLobby")
		{
			GameInstance->TransitionToLevel(FString("LV_TestLobby"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Target level name is empty for continent"));
		}
	}
}

void UCWidgetBase::OnBtnSetClicked()
{
	PlaySystemMessageAnim();
}

void UCWidgetBase::OnBtnExitClicked()
{
	UKismetSystemLibrary::QuitGame(this, GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit, false);
}
