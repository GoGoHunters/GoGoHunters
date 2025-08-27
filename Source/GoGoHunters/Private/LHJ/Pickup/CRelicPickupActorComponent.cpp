#include "LHJ/Pickup/CRelicPickupActorComponent.h"

#include "JMH/MH_GrabComp.h"

UCRelicPickupActorComponent::UCRelicPickupActorComponent()
{
}

void UCRelicPickupActorComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UCRelicPickupActorComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UCRelicPickupActorComponent::Pickup(USceneComponent* AttachTo, bool IsPulling)
{
	PendingGrabComponent->SetSimulatePhysics(true);
	Super::Pickup(AttachTo, IsPulling);
}

void UCRelicPickupActorComponent::Drop(USceneComponent* DropFrom)
{
	Super::Drop(DropFrom);
}

void UCRelicPickupActorComponent::GrabUsingForRelic()
{
	if (!OwnerActor) return;
	GrabComp->RelicGrab(OwnerActor);
}

void UCRelicPickupActorComponent::ReleaseUsingForRelic()
{
	if (!OwnerActor) return;
	GrabComp->RelicUnGrab(OwnerActor);
}
