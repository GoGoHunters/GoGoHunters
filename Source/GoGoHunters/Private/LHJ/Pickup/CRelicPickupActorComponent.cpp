#include "LHJ/Pickup/CRelicPickupActorComponent.h"

#include "EngineUtils.h"
#include "JMH/MH_GrabComp.h"
#include "LHM/Restore/RestorePuzzleActor.h"
#include "LHM/Restore/PieceActor.h"

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
	Super::Pickup(AttachTo, IsPulling);
}

void UCRelicPickupActorComponent::Drop(USceneComponent* DropFrom)
{
	Super::Drop(DropFrom);

	ARestorePuzzleActor* PuzzleActor = nullptr;
	for (TActorIterator<ARestorePuzzleActor> It(GetWorld()); It; ++It)
	{
		PuzzleActor = *It;
		if (PuzzleActor)
		{
			if (APieceActor* OwnerPiece = Cast<APieceActor>(OwnerActor))
			{
				PuzzleActor->TrySnapPiece(OwnerPiece);
			}
		}
		break;
	}
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
