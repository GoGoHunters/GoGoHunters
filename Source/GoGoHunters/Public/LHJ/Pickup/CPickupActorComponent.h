#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CPickupActorComponent.generated.h"


class AMH_VRPlayer;
class UMH_GrabComp;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GOGOHUNTERS_API UCPickupActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	virtual void Pickup(USceneComponent* AttachTo, bool IsPulling);
	virtual void Drop(USceneComponent* DropFrom);
	bool GetGrabActorSimulate() {return PendingGrabComponent->IsSimulatingPhysics();}
	void SetGrabActorScale(const FVector& Scale3D);

protected:
	UPROPERTY(VisibleAnywhere, Category="PickUp")
	TObjectPtr<UPrimitiveComponent> PendingGrabComponent = nullptr;
	UPROPERTY(EditAnywhere, Category="Pickup")
	FName PickupName = FName("Pickup");
	UPROPERTY(EditAnywhere, Category="Pickup")
	bool CanTwoHandGrab = false;
	UPROPERTY(EditAnywhere, Category="Pickup")
	float GrabPullSpeed = 5.0f;
	UPROPERTY(EditAnywhere, Category="Pickup")
	float MaxScalePercent = 1.5f;	
	UPROPERTY(EditAnywhere, Category="Pickup")
	float MinScalePercent = 0.5f;

	UPROPERTY(VisibleAnywhere, Category="Pickup")
	FVector MinScale3D;
	UPROPERTY(VisibleAnywhere, Category="Pickup")
	FVector MaxScale3D;
	
	UPROPERTY()
	TObjectPtr<AActor> OwnerActor = nullptr;
	UPROPERTY()
	TObjectPtr<AMH_VRPlayer> Player = nullptr;
	UPROPERTY()
	TObjectPtr<UMH_GrabComp> GrabComp = nullptr;
	UPROPERTY()
	TObjectPtr<USceneComponent> AttachComponent = nullptr;
	UPROPERTY()
	TObjectPtr<USceneComponent> FirstHandComponent = nullptr;
	UPROPERTY()
	TObjectPtr<USceneComponent> SecondHandComponent = nullptr;

	UCPickupActorComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	virtual void ReleaseUsingForRelic();
	virtual void GrabUsingForRelic();

private:
	bool bSetPlayerComp = false;
	bool bIsPulling = false;
	FRotator GrabRotation = FRotator::ZeroRotator;
	FName OriginProfileName;
	
	const FName GrabProfileName = FName("GrabbingObject");
	FCollisionResponseContainer GrabCollisionResponse;

	FVector OriginScale3D;
	FVector SecondHandAttachT;
};
