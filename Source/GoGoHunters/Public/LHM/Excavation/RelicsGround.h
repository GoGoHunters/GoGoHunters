// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RelicsGround.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGroundDugDelegate, FVector, ImpactLocation);

UCLASS()
class GOGOHUNTERS_API ARelicsGround : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARelicsGround();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UFUNCTION(BlueprintCallable)
	void SetShovelReference(class AShovelTool* NewShovelRef);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HeightMapPainter")
	class AShovelTool* Shovel_Ref;

	UFUNCTION(BlueprintCallable, Category = "Digging Events")
	void TriggerOnGroundDug(FVector ImpactLocation);

	UPROPERTY(BlueprintAssignable, Category = "Digging Events")
    FOnGroundDugDelegate OnGroundDug;
};
