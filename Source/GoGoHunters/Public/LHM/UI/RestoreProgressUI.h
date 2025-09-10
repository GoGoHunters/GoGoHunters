// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RestoreProgressUI.generated.h"

/**
 * 
 */
UCLASS()
class GOGOHUNTERS_API URestoreProgressUI : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	void SetSnappedImage();
	
private:
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* Txt_Percent;

	UPROPERTY(meta = (BindWidget))
	class UImage* Img_Snapped_1;
	UPROPERTY(meta = (BindWidget))
	class UImage* Img_Snapped_2;
	UPROPERTY(meta = (BindWidget))
	class UImage* Img_Snapped_3;
	UPROPERTY(meta = (BindWidget))
	class UImage* Img_Snapped_4;
	UPROPERTY(meta = (BindWidget))
	class UImage* Img_Snapped_5;
	UPROPERTY(meta = (BindWidget))
	class UImage* Img_Snapped_6;
	UPROPERTY(meta = (BindWidget))
	class UImage* Img_Snapped_7;
	UPROPERTY(meta = (BindWidget))
	class UImage* Img_Snapped_8;
	UPROPERTY(meta = (BindWidget))
	class UImage* Img_Snapped_9;

	TArray<class UImage*> SnappedImgs;

	UPROPERTY(meta = (BindWidget))
	class UImage* Img_1;
	UPROPERTY(meta = (BindWidget))
	class UImage* Img_2;
	UPROPERTY(meta = (BindWidget))
	class UImage* Img_3;
	UPROPERTY(meta = (BindWidget))
	class UImage* Img_4;
	UPROPERTY(meta = (BindWidget))
	class UImage* Img_5;
	UPROPERTY(meta = (BindWidget))
	class UImage* Img_6;
	UPROPERTY(meta = (BindWidget))
	class UImage* Img_7;
	UPROPERTY(meta = (BindWidget))
	class UImage* Img_8;
	UPROPERTY(meta = (BindWidget))
	class UImage* Img_9;

	TArray<class UImage*> BGImgs;
};
