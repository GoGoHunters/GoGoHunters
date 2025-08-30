#include "UIs/CollectingBook/CCollectingItem.h"
#include "Components/Button.h"
#include "UIs/CollectingBook/CCollectingBookWidget.h"

void UCCollectingItem::SetData(FCRelicCollectingBook InCollectingData)
{
	CollectingData = InCollectingData;
	SetCollectionWidget();
}

void UCCollectingItem::NativeConstruct()
{
	Super::NativeConstruct();

	Btn_Thumbnail->OnClicked.AddDynamic(this, &UCCollectingItem::ThumbnailButtonClicked);
	
	ThumbnailButtonStyle = Btn_Thumbnail->GetStyle();
}

void UCCollectingItem::ThumbnailButtonClicked()
{
	CollectingBookWidget->ShowRelicDetailPage(CollectingData);
}

void UCCollectingItem::SetCollectionWidget()
{
	if (CollectingData.IsDrop)
	{
		if (CollectingData.RelicDetailData.RelicImage)
			// 수집된 아이템인 경우 Normal 이미지 변경
			ChangeThumbnailImage(CollectingData.RelicDetailData.RelicImage);
	}
	else
	{
		if (CollectingData.RelicDetailData.RelicBlindImage)
			ChangeThumbnailImage(CollectingData.RelicDetailData.RelicBlindImage);
	}
}

void UCCollectingItem::ChangeThumbnailImage(UTexture2D* NewTexture)
{
	if (NewTexture && Btn_Thumbnail)
	{
		// 새로운 이미지로 Normal 상태의 이미지 변경
		FSlateBrush NormalBrush = ThumbnailButtonStyle.Normal;
		NormalBrush.SetResourceObject(NewTexture);
		// NormalBrush.ImageSize = FVector2D(NewTexture->GetSizeX(), NewTexture->GetSizeY());

		// ThumbnailButtonStyle의 Normal 상태 업데이트
		ThumbnailButtonStyle.Normal = NormalBrush;

		FSlateBrush HoverBrush = ThumbnailButtonStyle.Hovered;
		HoverBrush.SetResourceObject(NewTexture);
		ThumbnailButtonStyle.Hovered = HoverBrush;

		FSlateBrush PressedBrush = ThumbnailButtonStyle.Pressed;
		PressedBrush.SetResourceObject(NewTexture);
		ThumbnailButtonStyle.Pressed = PressedBrush;

		// 버튼에 새로운 스타일 적용
		Btn_Thumbnail->SetStyle(ThumbnailButtonStyle);
	}
}
