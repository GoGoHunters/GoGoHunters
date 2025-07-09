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
	
}

void ACRelicBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

