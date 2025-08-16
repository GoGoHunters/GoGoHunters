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
	void SetIsDigging(bool bNewIsDigging);
	
	UFUNCTION(BlueprintCallable, Category = "Digging")
	void OnDigActionCompleted();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Digging")
	bool bIsDigging;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Digging")
	bool bCanTriggerDigTrace = false;

	float CooldownTime = 1.0f;
	bool bWasDiggingLastFrame = false;
	FVector PreviousLocation;
	
	// 찔르기 동작 완료 후 재시작을 위한 변수들
	bool bDigActionCompleted = false;
	float DigActionCooldown = 0.5f; // 찔르기 동작 완료 후 0.5초 대기
	float DigActionTimer = 0.0f;

    /*UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Digging")
	bool bIsShovelLifting = false;
	bool bWasLiftingLastFrame = false;
	FVector PreviousSplatLocation;*/
    
	void UpdateDigSwingState(float DeltaTime);

	//UFUNCTION(BlueprintCallable)
	//void EvaluateShovelLiftMotion(float DeltaTime);

	//UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Digging")
	//bool bIsDigHoldState = false;
	//float HoldTimer = 0.0f;
	//const float MaxHoldTime = 3.0f; // 1초 동안 기다림

public:
	UFUNCTION()
	void UpdateFeedback(FVector ImpactLocation);
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Effects")
	class UHapticFeedbackEffect_Base* DigHapticFX;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Effects")
	class UNiagaraSystem* DigFX;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Effects")
    class USoundBase* SoundFX;
};
