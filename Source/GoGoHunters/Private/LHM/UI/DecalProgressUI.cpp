// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/UI/DecalProgressUI.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UDecalProgressUI::UpdateProgress(float Opacity)
{
    if(!ProgressBar || !Txt_Percent) return;

    float Progress = FMath::Clamp(Opacity, 0.0f, 1.0f);
    int32 Percent = FMath::RoundToInt(Progress * 100.0f);

    ProgressBar->SetPercent(Progress);

    if (Percent <= 0)
    {
        Txt_Percent->SetText(FText::FromString(TEXT("완료!")));
    }
    else
    {
		Txt_Percent->SetText(FText::Format(FText::FromString(TEXT("{0}%")), FText::AsNumber(Percent)));
    }
}

void UDecalProgressUI::SetProgressBarImage(int32 Index)
{
	if (!ProgressBar) return;

    // 백그라운드용 브러시 생성
    FSlateBrush BackgroundBrush;
    FString BrushPath = FString::Printf(TEXT("/Game/LHM/Texture/egg_%d.egg_%d"), Index, Index);
    BackgroundBrush.SetResourceObject(LoadObject<UTexture2D>(nullptr, *BrushPath));
    BackgroundBrush.ImageSize = FVector2D(100.0f, 100.0f);
    BackgroundBrush.DrawAs = ESlateBrushDrawType::Image;

    FProgressBarStyle BarStyle = ProgressBar->GetWidgetStyle();
    BarStyle.BackgroundImage = BackgroundBrush;
    ProgressBar->SetWidgetStyle(BarStyle);
}
