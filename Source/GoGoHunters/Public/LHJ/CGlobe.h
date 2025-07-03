#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CGlobe.generated.h"

class USphereComponent;

UCLASS()
class GOGOHUNTERS_API ACGlobe : public AActor
{
	GENERATED_BODY()
	
public:	
	UPROPERTY(EditDefaultsOnly)
	USphereComponent* SphereComonent;
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* Globe;

	UFUNCTION(BlueprintCallable)
	void Grab(USceneComponent* MotionController);
	UFUNCTION(BlueprintCallable)
	void Release();

private:
	ACGlobe();
	virtual void BeginPlay() override;	
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY()
	USceneComponent* GrabbedController = nullptr;
	FQuat LastControllerQuat;
	bool bIsGrabbed = false;

	UPROPERTY(EditDefaultsOnly, Category = "Sensitivity", meta = (AllowPrivateAccess = "true"))
	float RotationSensitivity = 1.0f;

	float AngularVelocityYaw = 0.f;
	bool bIsInertiaActive = false;

	void UpdateRotationFromController();
};
