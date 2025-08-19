#include "LHJ/CMuseumPlaceArea.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Utilities/CHelpers.h"
#include "DrawDebugHelpers.h"
#include "JMH/CMuseumComponent.h"
#include "LHJ/CRelicBase.h"

ACMuseumPlaceArea::ACMuseumPlaceArea()
{
	PrimaryActorTick.bCanEverTick = true;
	CHelpers::CreateComponent<UBoxComponent>(this, &BoxComponent, "BoxComponent");
	BoxComponent->SetCollisionProfileName(FName("PlaceActor"));
}

void ACMuseumPlaceArea::BeginPlay()
{
	Super::BeginPlay();
	MuseumComp = GetWorld()->GetFirstPlayerController()->GetPawn()->GetComponentByClass<UCMuseumComponent>();
}

void ACMuseumPlaceArea::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	for (UStaticMeshComponent* MeshComp : GridMeshComponents)
	{
		if (MeshComp)
		{
			MeshComp->DestroyComponent();
		}
	}
	for (UWidgetComponent* WidgetComp : DescriptionWidgetComponents)
	{
		if (WidgetComp)
		{
			WidgetComp->DestroyComponent();
		}
	}
	Super::EndPlay(EndPlayReason);
}

void ACMuseumPlaceArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MuseumComp && !StartCreateCell)
	{
		if (MuseumComp->bBeginPlayEnded)
		{
			CreateGrid();
			CreateGridMeshComponents();
			StartCreateCell = true;
		}
	}
	
	UpdateGridMeshComponents();
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

void ACMuseumPlaceArea::CreateGridMeshComponents()
{
	// 기존 메시 컴포넌트들 정리
	for (UStaticMeshComponent* MeshComp : GridMeshComponents)
	{
		if (MeshComp)
		{
			MeshComp->DestroyComponent();
		}
	}
	GridMeshComponents.Empty();

	// 큐브 스태틱 메시 로드
	if (!CubeMesh) return;

	// 기본 머터리얼 로드
	if (!BaseMaterial) return;

	// 각 그리드 셀에 대해 StaticMeshComponent 생성
	for (int32 i = 0; i < GridCells.Num(); ++i)
	{
		UStaticMeshComponent* MeshComp = NewObject<UStaticMeshComponent>(this);
		MeshComp->RegisterComponent();
		MeshComp->SetStaticMesh(CubeMesh);
		MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		MeshComp->SetVisibility(false); // 초기에는 숨김
		MeshComp->SetCastShadow(false);
		
		// 위치와 스케일 설정
		MeshComp->SetWorldLocation(GridCells[i].Center);
		MeshComp->SetWorldScale3D(FVector(CellSize / 100.f, CellSize / 100.f, 0.1f)); // 100은 기본 큐브 크기

		// 다이나믹 머티리얼 만들어서 색 추가
		UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, MeshComp);
		MeshComp->SetMaterial(0, DynamicMaterial);
		
		GridMeshComponents.Add(MeshComp);

		if (DescriptionWidget)
		{
			UWidgetComponent* WidgetComp = NewObject<UWidgetComponent>(this);
			WidgetComp->RegisterComponent();
			WidgetComp->SetWidget(CreateWidget(GetWorld(), DescriptionWidget));
			WidgetComp->SetWidgetSpace(EWidgetSpace::World);
			WidgetComp->SetDrawSize(DescWidgetDrawSize);
			WidgetComp->SetWorldLocation(GridCells[i].Center + MoveDescWidget);
			WidgetComp->SetWorldRotation(RotateDescWidget);
			WidgetComp->SetCastShadow(false);
			WidgetComp->SetWorldScale3D(DescWidgetScale);
			DescriptionWidgetComponents.Add(WidgetComp);
			UpdateDescriptionWidget(i, false);
		}
	}

	if (MuseumComp)
		MuseumComp->OnMakeGridCompleted.ExecuteIfBound();
}

void ACMuseumPlaceArea::UpdateGridMeshComponents() const
{
	if (MuseumComp && MuseumComp->GetMuseumState() == Decorate)
	{
		for (int32 i = 0; i < GridCells.Num(); ++i)
		{
			GridMeshComponents[i]->SetVisibility(true);
			FColor Color = GridCells[i].bOccupied ? FColor::Red : FColor::Green;
			
			// 색 적용 추가
			UMaterialInstanceDynamic* DynamicMaterial = Cast<UMaterialInstanceDynamic>(GridMeshComponents[i]->GetMaterial(0));
			if (DynamicMaterial)
			{
				DynamicMaterial->SetVectorParameterValue(TEXT("Color"), FLinearColor(Color));
			}
		}
	}
	else
	{
		// 데코모드가 아닐 때는 모든 메시 컴포넌트 숨김
		for (UStaticMeshComponent* MeshComp : GridMeshComponents)
		{
			if (MeshComp)
			{
				MeshComp->SetVisibility(false);
			}
		}
	}
}

void ACMuseumPlaceArea::UpdateDescriptionWidget(int32 idx, bool bUpdate, FCRelicData InRelicData, FCRelicDetailData InRelicDetailData)
{
	if (DescriptionWidgetComponents.Num() < idx + 1) return;;
	if (!DescriptionWidgetComponents[idx]) return;;
	UUserWidget* WBPDescriptionWidget = DescriptionWidgetComponents[idx]->GetWidget();
	if (!WBPDescriptionWidget) return;
			
	FName FunctionName(TEXT("UpdateData"));
	UFunction* Function = WBPDescriptionWidget->FindFunction(FunctionName);
	if (Function)
	{
		FCRelicDataParam param;
		param.RelicData = InRelicData;
		param.RelicDetailData = InRelicDetailData;
		param.IsUpdate = bUpdate;
		WBPDescriptionWidget->ProcessEvent(Function, &param);
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

void ACMuseumPlaceArea::SetPlaceRelicAtLocation(ACRelicBase* Relic, const FVector& WorldLocation, const int32 PlaceIdx)
{
	if (!Relic) return;
	FCRelicData PlaceRelicData = Relic->GetRelicData();
	FCRelicDetailData PlaceRelicDetailData = Relic->GetRelicDetailData();
	
	Relic->SetActorLocation(WorldLocation);
	Relic->SetActorScale3D(GridCells[PlaceIdx].Scale);
	Relic->SetActorRotation(FRotator::ZeroRotator);
	UpdateDescriptionWidget(PlaceIdx, true, PlaceRelicData, PlaceRelicDetailData);
}

void ACMuseumPlaceArea::PlaceRelicAt(const ACRelicBase* InPlaceRelic)
{
	const FVector PlaceRelicLocation = InPlaceRelic->GetActorLocation();
	FCRelicData PlaceRelicData = InPlaceRelic->GetRelicData();
	FCRelicDetailData PlaceRelicDetailData = InPlaceRelic->GetRelicDetailData();
	for (int32 i = 0; i < GridCells.Num(); ++i)
	{		
		if (FVector::Dist2D(GridCells[i].Center, PlaceRelicLocation) < CellSize * 0.5f)
		{
			GridCells[i].bOccupied = true;
			UpdateDescriptionWidget(i, true, PlaceRelicData, PlaceRelicDetailData);
			break;
		}
	}
}

FVector ACMuseumPlaceArea::FindEmptySlot(const FVector& FromLocation) const
{
	float MinDist = TNumericLimits<float>::Max();
	const FGridCell* ClosestCell = nullptr;

	for (const FGridCell& Cell : GridCells)
	{
		if (!Cell.bOccupied)
		{
			float Dist = FVector::Dist2D(Cell.Center, FromLocation);
			if (Dist < MinDist)
			{
				MinDist = Dist;
				ClosestCell = &Cell;
			}
		}
	}

	if (ClosestCell)
		return ClosestCell->Center;
	return FVector::ZeroVector;
}

void ACMuseumPlaceArea::UnregisterRelic(const ACRelicBase* Relic)
{
	if (!Relic) return;

	for (int32 i = 0; i < GridCells.Num(); ++i)
	{
		if (FVector::Dist2D(GridCells[i].Center, Relic->GetRelicPlaceLocation()) < CellSize * 0.5f)
		{
			GridCells[i].bOccupied = false;
			UpdateDescriptionWidget(i, false);
			break;
		}
	}
}

