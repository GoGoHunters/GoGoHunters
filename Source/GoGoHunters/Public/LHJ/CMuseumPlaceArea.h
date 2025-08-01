#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CMuseumPlaceArea.generated.h"

class ACRelicBase;
class UBoxComponent;
class UCMuseumComponent;

USTRUCT()
struct FGridCell
{
	GENERATED_BODY()

	FVector Center;
	bool bOccupied = false;
	FVector Scale = FVector(1.f); // 셀별 유물 스케일
};

UCLASS()
class GOGOHUNTERS_API ACMuseumPlaceArea : public AActor
{
	GENERATED_BODY()

public:
	// 모든 셀에 동일하게 적용할 유물 스케일
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	FVector CellUniformScale = FVector(1.f);

	// 유물 설치
	void SetPlaceRelicAtLocation(ACRelicBase* Relic, const FVector& WorldLocation);
	void PlaceRelicAt(const FVector& WorldLocation);
	
	// GridCells 접근용 Getter
	const TArray<FGridCell>& GetGridCells() const { return GridCells; }
	// 유물 설치 가능 여부
	bool CanPlaceRelicAt(const FVector& WorldLocation) const;

	FVector FindEmptySlot(const FVector& FromLocation) const;
	void UnregisterRelic(const ACRelicBase* Relic);

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

	UPROPERTY()
	TObjectPtr<UCMuseumComponent> MuseumComp;
	
	ACMuseumPlaceArea();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void CreateGrid();
	void DrawGridDebug() const;

	bool GetCell(const FVector& WorldLocation, FGridCell& outCell);
};
