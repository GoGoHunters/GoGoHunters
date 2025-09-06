#pragma once

#include "CoreMinimal.h"
#include "LHJ/Pickup/CPickupActorComponent.h"
#include "CRestorePickupActorComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GOGOHUNTERS_API UCRestorePickupActorComponent : public UCPickupActorComponent
{
	GENERATED_BODY()

private:
	FVector OriginLocation;
	FRotator OriginRotation;
	
	void GrabOverrideFunc() override;
	void ReleaseOverrideFunc() override;
};
