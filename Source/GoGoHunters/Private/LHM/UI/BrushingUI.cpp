// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/UI/BrushingUI.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/HorizontalBox.h"
#include "Components/VerticalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "LHM/UI/DecalProgressUI.h"
#include "LHM/Excavation/RelicsBase.h"
#include "Components/DecalComponent.h"

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
