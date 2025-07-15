#include "LHJ/CMuseumActorBase.h"

ACMuseumActorBase::ACMuseumActorBase()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ACMuseumActorBase::BeginPlay()
{
	Super::BeginPlay();
}

void ACMuseumActorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
