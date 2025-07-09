// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/UI/ExcavationUI.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"
#include "JMH/MH_VRPlayer.h"

void UExcavationUI::NativeConstruct()
{
	VRPlayer = Cast<AMH_VRPlayer>(UGameplayStatics::GetPlayerCharacter(this, 0));
	
	if (Btn_Tool1) Btn_Tool1->OnClicked.AddDynamic(this, &UExcavationUI::OnClick_DetectionTool);
	if (Btn_Tool2) Btn_Tool2->OnClicked.AddDynamic(this, &UExcavationUI::OnClick_ShovelTool);
	if (Btn_Tool3) Btn_Tool3->OnClicked.AddDynamic(this, &UExcavationUI::OnClick_BrushTool);
}

void UExcavationUI::OnClick_DetectionTool()
{
	if(VRPlayer) VRPlayer->ExcavationTool1();
}

void UExcavationUI::OnClick_ShovelTool()
{
	if (VRPlayer) VRPlayer->ExcavationTool2();
}

void UExcavationUI::OnClick_BrushTool()
{
	if (VRPlayer) VRPlayer->ExcavationTool3();
}
