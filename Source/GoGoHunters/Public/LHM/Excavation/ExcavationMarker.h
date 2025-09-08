// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExcavationMarker.generated.h"

UCLASS()
class GOGOHUNTERS_API AExcavationMarker : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AExcavationMarker();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	void ActivateMarker();

protected:
    UPROPERTY(EditAnywhere)
    class UNiagaraSystem* PulseEffect;

    UPROPERTY(EditAnywhere)
	class UMaterialInstanceDynamic* OutlineMat;

private:
	// Appearance animation settings
	UPROPERTY(EditAnywhere, Category = "Appearance|Animation")
	float AppearDurationSeconds = 0.6f;

	FVector TargetWorldScale = FVector(1.0f, 1.0f, 1.0f);

	// Drop-from-above effect settings
	UPROPERTY(EditAnywhere, Category = "Appearance|Animation")
	bool bEnableDropEffect = true;

	UPROPERTY(EditAnywhere, Category = "Appearance|Animation", meta = (EditCondition = "bEnableDropEffect", ClampMin = "0.0"))
	float DropHeight = 200.0f;
	
	// Appearance animation state
	bool bIsAppearing = false;
	float AppearingElapsedSeconds = 0.0f;
	FVector InitialWorldScale = FVector(0.0f, 0.0f, 0.0f);
	FVector TargetWorldLocation = FVector::ZeroVector;
	FVector DropStartLocation = FVector::ZeroVector;
};
