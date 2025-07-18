#include "LHJ/CMuseumPlaceArea.h"

#include "Components/BoxComponent.h"
#include "Utilities/CHelpers.h"
#include "DrawDebugHelpers.h"

ACMuseumPlaceArea::ACMuseumPlaceArea()
{
	PrimaryActorTick.bCanEverTick = true;
	CHelpers::CreateComponent<UBoxComponent>(this, &BoxComponent, "BoxComponent");
	BoxComponent->SetCollisionProfileName(FName("PlaceActor"));
}

void ACMuseumPlaceArea::BeginPlay()
{
	Super::BeginPlay();
	CreateGrid();
}

void ACMuseumPlaceArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	DrawGridDebug();
}

void ACMuseumPlaceArea::CreateGrid()
{
	GridCells.Empty();
	FVector Origin = BoxComponent->Bounds.Origin;
	FVector Extent = BoxComponent->Bounds.BoxExtent;
	float StartX = Origin.X - Extent.X;
	float StartY = Origin.Y - Extent.Y;

	for (int32 x = 0; x < GridXCount; ++x)
	{
		for (int32 y = 0; y < GridYCount; ++y)
		{
			FVector CellCenter = FVector(
				StartX + (x + 0.5f) * CellSize,
				StartY + (y + 0.5f) * CellSize,
				Origin.Z
			);
			FGridCell Cell;
			Cell.Center = CellCenter;
			Cell.Scale = CellUniformScale; // 모든 셀에 동일한 스케일 적용
			GridCells.Add(Cell);
		}
	}
}

void ACMuseumPlaceArea::DrawGridDebug() const
{
	if (!bShowGridDebug) return;
	for (const FGridCell& Cell : GridCells)
	{
		FColor Color = Cell.bOccupied ? FColor::Red : FColor::Green;
		DrawDebugBox(GetWorld(), Cell.Center, FVector(CellSize/2, CellSize/2, 10.f), Color, false, -1, 0, 2);
	}
}

bool ACMuseumPlaceArea::CanPlaceRelicAt(const FVector& WorldLocation) const
{
	for (const FGridCell& Cell : GridCells)
	{
		if (FVector::Dist2D(Cell.Center, WorldLocation) < CellSize * 0.5f)
		{
			return !Cell.bOccupied;
		}
	}
	return false;
}

void ACMuseumPlaceArea::PlaceRelicAt(const FVector& WorldLocation)
{
	for (FGridCell& Cell : GridCells)
	{
		if (FVector::Dist2D(Cell.Center, WorldLocation) < CellSize * 0.5f)
		{
			Cell.bOccupied = true;
			break;
		}
	}
}

void ACMuseumPlaceArea::SetGridDebugVisible(bool bVisible)
{
	bShowGridDebug = bVisible;
}

