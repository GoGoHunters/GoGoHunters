#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CTutorialData.generated.h"

USTRUCT(BlueprintType)
struct FCTutorialData : public FTableRowBase
{
	GENERATED_BODY()

	// 튜토리얼 고유 ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tutorial")
	FString StepID;

	// 이미지/애니메이션/사운드
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tutorial")
	UTexture2D* TutorialImage = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tutorial")
	UAnimMontage* TutorialAnimation = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tutorial")
	USoundBase* TutorialSound = nullptr;
};
