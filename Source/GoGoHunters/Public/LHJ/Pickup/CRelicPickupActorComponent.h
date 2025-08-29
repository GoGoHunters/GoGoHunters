#pragma once

#include "CoreMinimal.h"
#include "LHJ/Pickup/CPickupActorComponent.h"
#include "CRelicPickupActorComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GOGOHUNTERS_API UCRelicPickupActorComponent : public UCPickupActorComponent
{
	GENERATED_BODY()

public:
	void Pickup(USceneComponent* AttachTo, bool IsPulling) override;
	void Drop(USceneComponent* DropFrom) override;
	
private:
	UCRelicPickupActorComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GrabUsingForRelic() override;
	virtual void ReleaseUsingForRelic() override;
};
