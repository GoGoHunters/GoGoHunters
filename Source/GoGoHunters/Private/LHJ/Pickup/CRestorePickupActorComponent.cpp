#include "LHJ/Pickup/CRestorePickupActorComponent.h"

void UCRestorePickupActorComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!PendingGrabComponent) return;
	OriginLocation = PendingGrabComponent->GetComponentLocation();
	OriginRotation = PendingGrabComponent->GetComponentRotation();
}

void UCRestorePickupActorComponent::ReleaseOverrideFunc()
{
	PendingGrabComponent->SetupAttachment(OwnerActor->GetRootComponent());
	PendingGrabComponent->SetWorldLocationAndRotation(OriginLocation, OriginRotation);
	PendingGrabComponent->SetCollisionProfileName(FName("GrabbingObject"));
}
