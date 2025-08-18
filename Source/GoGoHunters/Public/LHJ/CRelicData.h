#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CRelicData.generated.h"

class ACMuseumPlaceArea;
class ACRelicBase;
/**
 * 
 */
USTRUCT(Atomic, BlueprintType)
struct FCRelicData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FText RelicName=FText::FromString(""); // 유물 이름 (FK)
	UPROPERTY(EditAnywhere)
	uint32 RelicTag = -1;
	UPROPERTY(EditAnywhere)
	TObjectPtr<ACMuseumPlaceArea> PlaceArea;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDateTime DropDate; // 획득 날짜,시간
	UPROPERTY(EditAnywhere)
	FTransform PlacedTransform; // 배치 위치
	UPROPERTY(EditAnywhere)
	bool IsPlace = false; // 박물관 배치 유무
};

USTRUCT(Atomic, BlueprintType)
struct FCRelicDetailData : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText RelicName; // 유물 이름 (PK)
	UPROPERTY(EditAnywhere)
	uint32 RelicTag = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText RelicDesc; // 유물 설명
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText ContinentName; // 대륙 이름
	UPROPERTY(EditAnywhere)
	UTexture2D* RelicImage; // 유물 이미지
	UPROPERTY(EditAnywhere)
	TSubclassOf<ACRelicBase> RelicActorClass; // 유물 액터 클래스
	UPROPERTY(EditAnywhere)
	bool IsDrop = false; // 획득 유무	
};

USTRUCT(Atomic, BlueprintType)
struct FCRelicDataParam : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FCRelicData RelicData;
	UPROPERTY(BlueprintReadOnly)
	FCRelicDetailData RelicDetailData;
	UPROPERTY(BlueprintReadOnly)
	bool IsUpdate = false;
};