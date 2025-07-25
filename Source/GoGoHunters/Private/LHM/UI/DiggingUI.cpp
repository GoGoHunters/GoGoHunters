// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/UI/DiggingUI.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UDiggingUI::UpdateUI(float Progress)
{
    DiggingProgressBar->SetPercent(Progress);
    UpdateStatusText(Progress);
}

void UDiggingUI::UpdateStatusText(float Progress)
{
    int32 DisplayPercent = (Progress >= 0.99f) ? 100 : (int32)(Progress * 100.0f);
    FString Status = FString::Printf(TEXT("발굴 진행률: %d%%"), DisplayPercent);
    StatusText->SetText(FText::FromString(Status));
}
