// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShovelTool.generated.h"

UCLASS()
class GOGOHUNTERS_API AShovelTool : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShovelTool();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ShovelTool")
	class UStaticMeshComponent* ShovelMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ShovelTool")
	class USceneComponent* SplatPoint;

	UFUNCTION(BlueprintCallable, Category = "Digging")
	void SetIsDigging(bool bNewIsDigging) { bIsDigging = bNewIsDigging; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Digging")
	bool bIsDigging;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Digging")
	bool bCanTriggerDigTrace = false;
	bool bWasDiggingLastFrame = false;
	FVector PreviousLocation;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Digging")
	bool bIsShovelLifting = false;
	bool bWasLiftingLastFrame = false;
	FVector PreviousSplatLocation;
    
	void UpdateDigSwingState(float DeltaTime);
	UFUNCTION(BlueprintCallable)
	void EvaluateShovelLiftMotion(float DeltaTime);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Digging")
	bool bIsDigHoldState = false;
	float HoldTimer = 0.0f;
	const float MaxHoldTime = 3.0f; // 1초 동안 기다림

public:
	UFUNCTION()
	void UpdateFeedback(FVector ImpactLocation);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Digging")
	class UHapticFeedbackEffect_Base* DigHapticEffect;
};
