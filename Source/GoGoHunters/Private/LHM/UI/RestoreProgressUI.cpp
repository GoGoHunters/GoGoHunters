// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/UI/RestoreProgressUI.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void URestoreProgressUI::NativeConstruct()
{
    SnappedImgs =
    {
		Img_Snapped_1,
		Img_Snapped_2,
		Img_Snapped_3,
		Img_Snapped_4,
		Img_Snapped_5,
		Img_Snapped_6,
		Img_Snapped_7,
		Img_Snapped_8,
        Img_Snapped_9
    };

    for (UImage* Img : SnappedImgs)
    {
        if (Img) Img->SetVisibility(ESlateVisibility::Hidden);
    }

    BGImgs =
    {
        Img_1,
        Img_2,
        Img_3,
        Img_4,
        Img_5,
        Img_6,
        Img_7,
        Img_8,
		Img_9
    };

    for (UImage* Img : BGImgs)
    {
        if (Img) Img->SetRenderOpacity(0.3f);
    }
}

void URestoreProgressUI::SetSnappedImage()
{
    if (SnappedImgs.Num() == 0) return;

    int32 SnappedImgsNum = SnappedImgs.Num();

    for (int32 i = 0; i < SnappedImgs.Num(); ++i)
    {
        UImage* Img = SnappedImgs[i];
        if (!Img) continue;
        if (Img->GetVisibility() == ESlateVisibility::Visible) continue;
        Img->SetVisibility(ESlateVisibility::Visible);

        if(i == SnappedImgsNum - 1)
        {
            Txt_Percent->SetText(FText::FromString(TEXT("복원 완료!")));
            break;
		}
        else
        {
            Txt_Percent->SetText(FText::Format(FText::FromString(TEXT("복원 진행률: {0}/{1}")),
                FText::AsNumber(i + 1), 
                FText::AsNumber(SnappedImgsNum)
            ));
            break;
        }
    }
}
