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
    virtual void NativeConstruct() override;
    
    void UpdateProgress(float Opacity);
    void SetCollectedImage(bool bCollected);

    UPROPERTY(meta = (BindWidget))
    class UProgressBar* ProgressBar;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* Txt_Percent;
    
	UPROPERTY(meta = (BindWidget))
	class UImage* Img_Collected_1;
	UPROPERTY(meta = (BindWidget))
	class UImage* Img_Collected_2;
	UPROPERTY(meta = (BindWidget))
	class UImage* Img_Collected_3;
	UPROPERTY(meta = (BindWidget))
	class UImage* Img_Collected_4;
	UPROPERTY(meta = (BindWidget))
	class UImage* Img_Collected_5;
	UPROPERTY(meta = (BindWidget))
	class UImage* Img_Collected_6;
	UPROPERTY(meta = (BindWidget))
	class UImage* Img_Collected_7;
	UPROPERTY(meta = (BindWidget))
	class UImage* Img_Collected_8;

    TArray<class UImage*> CollectedImgs;

// 개발자 키
    void SetAllCollected();

#pragma region DecalProgressUI
protected:
    virtual void BeginDestroy() override;

public:
    // 데칼별 위젯 생성 함수 (Decal 배열 전달)
    void CreateDecalWidgets(const TArray<UDecalComponent*>& Decals);
    // 오파시티에 따라 UI 갱신 함수 (Decal 포인터 기반)
    void UpdateDecalProgress(UDecalComponent* UpdatedDecal, float Opacity);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI")
    TSubclassOf<class UDecalProgressUI> DecalProgressUIClass;

    UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* DecalHorizontalBox;

	TMap<UStaticMeshComponent*, UDecalProgressUI*> GetMeshToWidgetMap() const { return MeshToWidgetMap; }

private:
    TMap<UStaticMeshComponent*, UDecalProgressUI*> MeshToWidgetMap;
    TMap<UStaticMeshComponent*, TArray<UDecalComponent*>*> MeshToDecalsMap;
    TMap<UDecalComponent*, float> CachedOpacities;
#pragma endregion drop

};
