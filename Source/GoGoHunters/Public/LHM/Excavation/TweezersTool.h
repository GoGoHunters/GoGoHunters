// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TweezersTool.generated.h"

UCLASS()
class GOGOHUNTERS_API ATweezersTool : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATweezersTool();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void PickUpRelic();

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void SetIsPickingUp(bool _bIsPickingUp);

	void SetAttachBase(USceneComponent* InAttachBase);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UStaticMeshComponent* TweezersMeshL;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UStaticMeshComponent* TweezersMeshR;

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* PickupBox;

	UPROPERTY(VisibleAnywhere)
	class USceneComponent* PickupPoint;

	bool bIsPickingUp = false;

	USceneComponent* AttachBase = nullptr;
	FVector PreviousAttachLocation;
	FVector LastAttachLocation;
	bool bHasJustDropped = false;

	class UMotionControllerComponent* OwnerMC;

private:
	UPROPERTY()
	class ARelicsBase* PickedRelic;

	UPROPERTY()
	class ARelicsBase* RelicCandidate = nullptr;

	UPROPERTY()
	class UStaticMeshComponent* CandidateMesh = nullptr;
};
