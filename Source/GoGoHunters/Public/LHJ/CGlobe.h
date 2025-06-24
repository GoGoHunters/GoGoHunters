#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CGlobe.generated.h"

UCLASS()
class GOGOHUNTERS_API ACGlobe : public AActor
{
	GENERATED_BODY()
	
public:	
	void Grab(USceneComponent* MotionController);
	void Release();

private:
	ACGlobe();
	virtual void BeginPlay() override;	
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY()
	USceneComponent* GrabbedController = nullptr;
	FQuat LastControllerQuat;
	bool bIsGrabbed = false;

	void UpdateRotationFromController();
};
