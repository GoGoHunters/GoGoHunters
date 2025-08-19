#pragma once

#include "CoreMinimal.h"
#include "CRelicData.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "CMuseumPlaceArea.generated.h"

struct FCRelicData;
class UWidgetComponent;
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
	void SetPlaceRelicAtLocation(ACRelicBase* Relic, const FVector& WorldLocation, int32 PlaceIdx);
	void PlaceRelicAt(const ACRelicBase* InPlaceRelic);
	
	// GridCells 접근용 Getter
	const TArray<FGridCell>& GetGridCells() const { return GridCells; }
	// 유물 설치 가능 여부
	bool CanPlaceRelicAt(const FVector& WorldLocation) const;

	FVector FindEmptySlot(const FVector& FromLocation) const;
	void UnregisterRelic(const ACRelicBase* Relic);

private:
	bool StartCreateCell = false;
	
	UPROPERTY()
	UBoxComponent* BoxComponent;

	UPROPERTY(VisibleAnywhere)
	TArray<FGridCell> GridCells;

	UPROPERTY(VisibleInstanceOnly)
	TArray<UStaticMeshComponent*> GridMeshComponents;
	UPROPERTY(VisibleInstanceOnly)
	TArray<UWidgetComponent*> DescriptionWidgetComponents;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	int32 GridXCount = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	int32 GridYCount = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	float CellSize = 100.f;

	UPROPERTY()
	TObjectPtr<UCMuseumComponent> MuseumComp;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UStaticMesh> CubeMesh;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UMaterial> BaseMaterial;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> DescriptionWidget;

	UPROPERTY(EditAnywhere)
	FVector DescWidgetScale = FVector(1.f);
	UPROPERTY(EditAnywhere)
	FVector2D DescWidgetDrawSize = FVector2D(200, 100);
	UPROPERTY(EditAnywhere)
	FVector MoveDescWidget = FVector(0, -120.f, 0);
	UPROPERTY(EditAnywhere)
	FRotator RotateDescWidget = FRotator(0, -90, 0);
	
	
	ACMuseumPlaceArea();
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;

	void CreateGrid();
	void CreateGridMeshComponents();
	void UpdateGridMeshComponents() const;
	void UpdateDescriptionWidget(int32 idx, bool bUpdate, FCRelicData InRelicData = FCRelicData(), FCRelicDetailData InRelicDetailData = FCRelicDetailData());
};
