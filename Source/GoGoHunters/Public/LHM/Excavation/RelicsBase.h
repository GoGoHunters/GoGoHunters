// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RelicsBase.generated.h"

UCLASS()
class GOGOHUNTERS_API ARelicsBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARelicsBase();

protected:
	virtual void PostInitializeComponents() override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* RelicMesh;

// 탐지
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AExcavationMarker> MarkerClass;

	UPROPERTY()
	class AExcavationMarker* Marker;

// 붓 데칼
	UPROPERTY(VisibleAnywhere)
	TArray<class UDecalComponent*> DustDecals;

	UPROPERTY(VisibleAnywhere)
	TMap<UDecalComponent*, UMaterialInstanceDynamic*> DecalMIDs;

	UPROPERTY(EditAnywhere)
	FName OpacityParameterName = TEXT("Opacity");

	UPROPERTY(EditAnywhere)
	float CurrentOpacity = 1.0f;

	void ReduceDustOpacity(const FVector& BrushLocation, float Amount);
};
