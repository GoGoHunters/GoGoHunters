#include "LHJ/Pickup/CRestorePickupActorComponent.h"

void UCRestorePickupActorComponent::GrabOverrideFunc()
{
	OriginLocation = PendingGrabComponent->GetComponentLocation();
	OriginRotation = PendingGrabComponent->GetComponentRotation();
}

void UCRestorePickupActorComponent::ReleaseOverrideFunc()
{
	PendingGrabComponent->SetupAttachment(OwnerActor->GetRootComponent());
	PendingGrabComponent->SetWorldLocationAndRotation(OriginLocation, OriginRotation);
	PendingGrabComponent->SetCollisionProfileName(FName("GrabbingObject"));
}
