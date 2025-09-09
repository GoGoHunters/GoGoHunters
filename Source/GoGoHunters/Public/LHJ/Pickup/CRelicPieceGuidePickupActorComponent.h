#pragma once

#include "CoreMinimal.h"
#include "LHJ/Pickup/CPickupActorComponent.h"
#include "CRelicPieceGuidePickupActorComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GOGOHUNTERS_API UCRelicPieceGuidePickupActorComponent : public UCPickupActorComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly, Category = "Pickup|Sensitivity", meta = (AllowPrivateAccess = "true"))
	float RotationSensitivity = 1.0f;
	
	FQuat LastControllerQuat;
	bool bIsInertiaActive = false;
	float AngularVelocityYaw = 0.f;

	UCRelicPieceGuidePickupActorComponent();
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GrabOverrideFunc() override;
	virtual void ReleaseOverrideFunc() override;
	void UpdateRotationFromController();	
};
