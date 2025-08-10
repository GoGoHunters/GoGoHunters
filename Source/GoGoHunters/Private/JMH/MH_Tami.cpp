// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_Tami.h"

#include "Components/Button.h"
#include "UIs/CWidgetBase.h"

void UMH_Tami::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_Audio)
		Btn_Audio->OnPressed.AddDynamic(this, &UMH_Tami::Btn_AudioPressed);
	if (Btn_TamiExit)
		Btn_TamiExit->OnClicked.AddDynamic(this, &UMH_Tami::Btn_TamiExitClicked);
}

void UMH_Tami::Btn_AudioPressed()
{
	
}

void UMH_Tami::Btn_TamiExitClicked()
{
	
}
