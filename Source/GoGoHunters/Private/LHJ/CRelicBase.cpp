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