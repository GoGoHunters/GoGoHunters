#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LHJ/CRelicData.h"
#include "CCollectingItem.generated.h"

class UCCollectingBookWidget;
class UButton;
class UTexture2D;
/**
 * 
 */
UCLASS()
class GOGOHUNTERS_API UCCollectingItem : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetData(FCRelicCollectingBook InCollectingData);

	void SetCollectionWidget(UCCollectingBookWidget* InCollectingBookWidget)
	{
		CollectingBookWidget = InCollectingBookWidget;
	}

private:
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UButton> Btn_Thumbnail;

	UPROPERTY()
	FButtonStyle ThumbnailButtonStyle;
	UPROPERTY()
	FCRelicCollectingBook CollectingData;
	UPROPERTY()
	TObjectPtr<UCCollectingBookWidget> CollectingBookWidget;

	virtual void NativeConstruct() override;
	UFUNCTION()
	void ThumbnailButtonClicked();

	void SetCollectionWidget();

	// ThumbnailButtonStyle의 Normal 이미지를 변경하는 함수
	void ChangeThumbnailImage(UTexture2D* NewTexture);
};
