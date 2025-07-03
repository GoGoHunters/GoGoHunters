// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/Excavation/DetectionUI.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UDetectionUI::UpdateUI(float Progress)
{
    DetectionProgressBar->SetPercent(Progress / 100.f);
    UpdateStatusText(Progress);
}

void UDetectionUI::UpdateStatusText(float Progress)
{
    FString Status;
	if (Progress < 30.f)
		Status = TEXT("토양 분석 중...");
	else if (Progress < 70.f)
		Status = TEXT("지리적 데이터 패턴 분석 중...");
	else if (Progress < 100.f)
		Status = TEXT("발굴 위치 식별 중...");
	else
		Status = TEXT("탐지 완료!");

    StatusText->SetText(FText::FromString(Status));
}
