// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/UI/BrushingUI.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/HorizontalBox.h"
#include "Components/VerticalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/DecalComponent.h"
#include "LHM/UI/DecalProgressUI.h"
#include "LHM/Excavation/RelicsBase.h"

void UBrushingUI::NativeConstruct()
{
    CollectedImgs =
    { 
        Img_Collected_1, 
        Img_Collected_2, 
        Img_Collected_3, 
        Img_Collected_4,
        Img_Collected_5,
        Img_Collected_6, 
        Img_Collected_7, 
        Img_Collected_8 
    };

    for (UImage* Img : CollectedImgs)
    {
        if (Img) Img->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UBrushingUI::UpdateProgress(float Opacity)
{
    if (!ProgressBar || !Txt_Percent) return;

	float Progress = FMath::Clamp(1.0f - Opacity, 0.0f, 1.0f);
    int32 Percent = FMath::RoundToInt(Progress * 100.0f);

    ProgressBar->SetPercent(Progress);

    FText Txt;

    if (Percent >= 100) Txt = FText::FromString(TEXT("먼지 제거 완료!"));
    else Txt = FText::Format(FText::FromString(TEXT("먼지 제거 진행률: {0}%")), FText::AsNumber(Percent));

	Txt_Percent->SetText(Txt);
}

void UBrushingUI::SetCollectedImage(bool bCollected)
{
    if (CollectedImgs.Num() == 0) return;

	int32 CollectedImgsNum = CollectedImgs.Num();

    if (bCollected)
	{
        //for (UImage* Img : CollectedImgs)
        for (int32 i = 0; i < CollectedImgs.Num(); ++i)
        {
			UImage* Img = CollectedImgs[i];
            if (!Img) continue;
            if (Img->GetVisibility() == ESlateVisibility::Visible) continue;
            Img->SetVisibility(ESlateVisibility::Visible);
            Txt_Percent->SetText(FText::Format(FText::FromString(TEXT("수집 진행률: {0}/{1}")), FText::AsNumber(i+1), FText::AsNumber(CollectedImgsNum)));
            break;
		}
    }
    else
    {
        for (int32 i = CollectedImgs.Num() - 1; i >= 0; --i)
        {
            UImage* Img = CollectedImgs[i];
            if (Img && Img->GetVisibility() == ESlateVisibility::Visible)
            {
                Img->SetVisibility(ESlateVisibility::Hidden);
                Txt_Percent->SetText(FText::Format(FText::FromString(TEXT("수집 진행률 {0}/{1}")), FText::AsNumber(i), FText::AsNumber(CollectedImgsNum)));
                break;
            }
        }
    }
}

void UBrushingUI::SetAllCollected()
{
    for (UImage* Img : CollectedImgs) Img->SetVisibility(ESlateVisibility::Visible);
	Txt_Percent->SetText(FText::FromString(TEXT("수집 완료!")));
}

void UBrushingUI::CreateDecalWidgets(const TArray<UDecalComponent*>& Decals)
{
    if (!DecalHorizontalBox) return;

    DecalHorizontalBox->ClearChildren();
    MeshToWidgetMap.Empty();
    CachedOpacities.Empty();

    // 메모리 정리
    for (auto& Pair : MeshToDecalsMap)
    {
        delete Pair.Value;
    }
    MeshToDecalsMap.Empty();

    for (UDecalComponent* Decal : Decals)
    {
        if (!Decal) continue;

        ARelicsBase* Relic = Cast<ARelicsBase>(Decal->GetOwner());
        if (!Relic) continue;

        UStaticMeshComponent* RelicMesh = Relic->GetRelicMeshByDecal(Decal);
        if (!RelicMesh) continue;

        // 위젯 생성은 유물당 한 번만
        if (!MeshToWidgetMap.Contains(RelicMesh))
        {
            if (UDecalProgressUI* Widget = CreateWidget<UDecalProgressUI>(this, DecalProgressUIClass))
            {
                Widget->SetProgressBarImage(MeshToWidgetMap.Num() + 1);
                Widget->UpdateProgress(1.0f);
                DecalHorizontalBox->AddChildToHorizontalBox(Widget);
                MeshToWidgetMap.Add(RelicMesh, Widget);

            }

            // 데칼 배열 할당
            MeshToDecalsMap.Add(RelicMesh, new TArray<UDecalComponent*>());
        }

        MeshToDecalsMap[RelicMesh]->Add(Decal);
		CachedOpacities.Add(Decal, 1.0f);
    }
}

void UBrushingUI::UpdateDecalProgress(UDecalComponent* UpdatedDecal, float Opacity)
{
    ARelicsBase* Relic = Cast<ARelicsBase>(UpdatedDecal->GetOwner());
    if (!Relic) return;

    UStaticMeshComponent* Mesh = Relic->GetRelicMeshByDecal(UpdatedDecal);
    if (!Mesh || !MeshToDecalsMap.Contains(Mesh)) return;

    CachedOpacities.FindOrAdd(UpdatedDecal) = Opacity;

    const TArray<UDecalComponent*>* Decals = MeshToDecalsMap[Mesh];
    float TotalOpacity = 0.f;
    int32 Count = 0;

    for (UDecalComponent* Decal : *Decals)
    {
        if (CachedOpacities.Contains(Decal))
        {
            TotalOpacity += CachedOpacities[Decal];
            Count++;
        }
    }

    float AvgOpacity = Count > 0 ? TotalOpacity / Count : 0.0f;

    if (UDecalProgressUI* Widget = MeshToWidgetMap[Mesh])
    {
        Widget->UpdateProgress(AvgOpacity);
    }
}

void UBrushingUI::BeginDestroy()
{
    Super::BeginDestroy();

    for (auto& Pair : MeshToDecalsMap)
    {
        delete Pair.Value;
    }
    MeshToDecalsMap.Empty();
}
