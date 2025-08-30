#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CRelicData.generated.h"

class ACRelicPlaceActor;
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
	TObjectPtr<ACRelicPlaceActor> PlaceArea;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName CollectorName = NAME_None;
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
	FText RelicToolTipDesc; // 유물 설명
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText ContinentName; // 대륙 이름
	UPROPERTY(EditAnywhere)
	UTexture2D* RelicImage; // 유물 이미지
	UPROPERTY(EditAnywhere)
	UTexture2D* RelicBlindImage; // 미획득 유물 이미지
	UPROPERTY(EditAnywhere)
	TSubclassOf<ACRelicBase> RelicActorClass; // 유물 액터 클래스	
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

USTRUCT(Atomic, BlueprintType)
struct FCRelicPlaceActorParam : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	ACRelicPlaceActor* RelicPlaceActor;
};

USTRUCT(Atomic, BlueprintType)
struct FCRelicCollectingBook : public FTableRowBase
{
	GENERATED_BODY()

	/**
	 * DropDate 획득 날짜 기록
	 *
	 * GameInstance에서 데이터 테이블 읽을 때
	 * 만약 도감 SaveData가 없다면 새로 만들고
	 * 있다면 기존 데이터랑 비교하여
	 * RelicTag가 있다면 데이터 업데이트를 하고, 없다면 추가한다.
	 */
	UPROPERTY(BlueprintReadOnly)
	FCRelicDetailData RelicDetailData;
	UPROPERTY(BlueprintReadOnly)
	FDateTime DropDate; // 획득 날짜,시간
	UPROPERTY()
	bool IsDrop = false; // 획득 유무
	UPROPERTY()
	bool IsRecover = false; // 복구 유무
};
