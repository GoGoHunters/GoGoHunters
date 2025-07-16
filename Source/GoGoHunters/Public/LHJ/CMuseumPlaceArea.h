#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CMuseumPlaceArea.generated.h"

class UBoxComponent;

USTRUCT()
struct FGridCell
{
	GENERATED_BODY()

	FVector Center;
	bool bOccupied = false;
};

UCLASS()
class GOGOHUNTERS_API ACMuseumPlaceArea : public AActor
{
	GENERATED_BODY()

public:

private:
	UPROPERTY()
	UBoxComponent* BoxComponent;

	UPROPERTY(VisibleAnywhere)
	TArray<FGridCell> GridCells;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	int32 GridXCount = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	int32 GridYCount = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	float CellSize = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	bool bShowGridDebug = false;

	ACMuseumPlaceArea();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void CreateGrid();
	void DrawGridDebug() const;

	// 유물 설치 가능 여부
	bool CanPlaceRelicAt(const FVector& WorldLocation) const;
	// 유물 설치
	void PlaceRelicAt(const FVector& WorldLocation);

	// 디버그용 그리드 표시 토글
	void SetGridDebugVisible(bool bVisible);
};
