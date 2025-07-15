#include "LHJ/CMuseumPlaceArea.h"

#include "Components/BoxComponent.h"
#include "Utilities/CHelpers.h"

ACMuseumPlaceArea::ACMuseumPlaceArea()
{
	PrimaryActorTick.bCanEverTick = true;
	CHelpers::CreateComponent<UBoxComponent>(this, &BoxComponent, "BoxComponent");
	BoxComponent->SetCollisionProfileName(FName("PlaceActor"));
}

void ACMuseumPlaceArea::BeginPlay()
{
	Super::BeginPlay();
}

void ACMuseumPlaceArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

