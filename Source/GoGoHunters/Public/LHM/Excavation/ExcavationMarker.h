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
	class UStaticMeshComponent* FlagMesh;

    UPROPERTY(EditAnywhere)
	class UMaterialInstanceDynamic* OutlineMat;
};
