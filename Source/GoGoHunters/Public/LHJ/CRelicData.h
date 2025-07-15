#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CRelicData.generated.h"

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
	FDateTime DropDate; // 획득 날짜,시간
	UPROPERTY(BlueprintReadWrite)
	FTransform PlacedTransform; // 배치 위치
	UPROPERTY(EditAnywhere)
	bool IsPlace = false; // 박물관 배치 유무
};

USTRUCT(Atomic, BlueprintType)
struct FCRelicDetailData : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	FText RelicName; // 유물 이름 (PK)
	UPROPERTY(EditAnywhere)
	FText RelicDesc; // 유물 설명
	UPROPERTY(EditAnywhere)
	FText ContinentName; // 대륙 이름
	UPROPERTY(EditAnywhere)
	UTexture2D* RelicImage; // 유물 이미지
	UPROPERTY(EditAnywhere)
	TSubclassOf<ACRelicBase> RelicActorClass; // 유물 액터 클래스
	UPROPERTY(EditAnywhere)
	bool IsDrop = false; // 획득 유무	
};
