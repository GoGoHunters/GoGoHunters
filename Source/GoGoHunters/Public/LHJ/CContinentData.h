#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CContinentData.generated.h"

USTRUCT(Atomic,BlueprintType)
struct FCContinentData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FString Key;
	UPROPERTY(EditAnywhere)
	FString ContinentName;	// 대륙 이름
	UPROPERTY(EditAnywhere)
	FString ContinentDesc;	// 대륙 설명
	UPROPERTY(EditAnywhere)
	TArray<FString> RelicsArray; // 등장 유물 종류
	UPROPERTY(EditAnywhere)
	bool UseJoin; // 참가 버튼 사용 유무
	UPROPERTY(EditAnywhere)
	FString TargetLevelName; // 이동할 레벨 이름
};
