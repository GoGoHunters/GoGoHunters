#include "LHJ/CRelicBase.h"

#include "Utilities/CHelpers.h"

ACRelicBase::ACRelicBase()
{
	PrimaryActorTick.bCanEverTick = true;
	CHelpers::CreateComponent<UStaticMeshComponent>(this, &RelicMesh, "RelicMesh");
}

void ACRelicBase::BeginPlay()
{
	Super::BeginPlay();
	RelicMaterial = RelicMesh->GetMaterial(0);
}

void ACRelicBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACRelicBase::SetRelicMaterial(UMaterialInterface* NewRelicMaterial)
{
	if (!NewRelicMaterial) return;
	RelicMesh->SetMaterial(0, NewRelicMaterial);
}

void ACRelicBase::InitializeAsset(const FCRelicData& InRelicData, const FCRelicDetailData& InRelicDetailData)
{
	RelicData = InRelicData;
	RelicDetailData = InRelicDetailData;
}

void ACRelicBase::SetRelicMaterial()
{
	if(!RelicMaterial) return;
	RelicMesh->SetMaterial(0, RelicMaterial);
}
