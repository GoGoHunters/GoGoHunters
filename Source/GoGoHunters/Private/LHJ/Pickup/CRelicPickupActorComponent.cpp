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

	if (bInvokedFromDrop)
	{
		return;
	}

	ARestorePuzzleActor* PuzzleActor = nullptr;
	for (TActorIterator<ARestorePuzzleActor> It(GetWorld()); It; ++It)
	{
		PuzzleActor = *It;
		if (PuzzleActor)
		{
			if (APieceActor* OwnerPiece = Cast<APieceActor>(OwnerActor))
			{
				PuzzleActor->PlaySnapFeedback(OwnerPiece);
			}
		}
		break;
	}
}

void UCRelicPickupActorComponent::Drop(USceneComponent* DropFrom)
{
	bInvokedFromDrop = true;
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

	bInvokedFromDrop = false;
}

void UCRelicPickupActorComponent::GrabOverrideFunc()
{
	if (!OwnerActor) return;
	GrabComp->RelicGrab(OwnerActor);
}

void UCRelicPickupActorComponent::ReleaseOverrideFunc()
{
	if (!OwnerActor) return;
	GrabComp->RelicUnGrab(OwnerActor);
}
