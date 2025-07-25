// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BrushingUI.generated.h"

/**
 * 
 */
UCLASS()
class GOGOHUNTERS_API UBrushingUI : public UUserWidget
{
	GENERATED_BODY()

public:
    // 데칼별 위젯 생성 함수 (Decal 배열 전달)
    void CreateDecalWidgets(const TArray<UDecalComponent*>& Decals);
    // 오파시티에 따라 UI 갱신 함수 (Decal 포인터 기반)
    void UpdateDecalProgress(UDecalComponent* UpdatedDecal, float Opacity);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI")
    TSubclassOf<class UDecalProgressUI> DecalProgressUIClass;

    UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* DecalHorizontalBox;

protected:
    virtual void BeginDestroy() override;

public:
	TMap<UStaticMeshComponent*, UDecalProgressUI*> GetMeshToWidgetMap() const { return MeshToWidgetMap; }

private:
    TMap<UStaticMeshComponent*, UDecalProgressUI*> MeshToWidgetMap;
    TMap<UStaticMeshComponent*, TArray<UDecalComponent*>*> MeshToDecalsMap;
    TMap<UDecalComponent*, float> CachedOpacities;

};
