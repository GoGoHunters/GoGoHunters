#include "LHJ/Pickup/CRestorePickupActorComponent.h"
#include "Kismet/GameplayStatics.h"
#include "LHM/Restore/RestorePuzzleActor.h"

void UCRestorePickupActorComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!PendingGrabComponent) return;

	TArray<AActor*> Found;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARestorePuzzleActor::StaticClass(), Found);

	if (Found.Num() > 0)
	{
		ARestorePuzzleActor* PuzzleActor = Cast<ARestorePuzzleActor>(Found[0]);

		OriginLocation = PuzzleActor->GetRootComponent()->GetComponentLocation() + FVector(0, 0, 25.5f);
		OriginRotation = PuzzleActor->GetRootComponent()->GetComponentRotation();
	}
}

void UCRestorePickupActorComponent::ReleaseOverrideFunc()
{
	PendingGrabComponent->SetupAttachment(OwnerActor->GetRootComponent());
	PendingGrabComponent->SetWorldLocationAndRotation(OriginLocation, OriginRotation);
	PendingGrabComponent->SetCollisionProfileName(FName("GrabbingObject"));
}
