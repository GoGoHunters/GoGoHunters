// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShovelTool.generated.h"

// 실제 땅파기 패턴을 위한 상태 관리
UENUM(BlueprintType)
enum class EDigPatternState : uint8
{
	Idle,
	Stabbing,    // 내리꽂기 중
	Lifting      // 위로 퍼내기 중
};

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
	EDigPatternState CurrentDigState = EDigPatternState::Idle;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Digging")
	bool bCanTriggerDigTrace = false;

	// 내리꽂기 관련 변수들
	FVector StabStartLocation;
	float StabStartTime = 0.0f;
	float MaxStabTime = 2.0f;  // 2초 내에 완료해야 함
	float MinStabDepth = 5.0f; // 최소 15cm 깊이
	//float MaxStabDepth = 100.0f; // 최대 40cm 깊이
	bool bReachedMinDepth = false;
	
	// 위로 퍼내기 관련 변수들
	float LiftStartTime = 0.0f;
	float MaxLiftTime = 1.5f;  // 1.5초 내에 완료해야 함
	float MinLiftHeight = 5.0f; // 최소 10cm 위로 올라가야 함
	
	// 기존 변수들
	float CooldownTime = 1.0f;
	bool bWasDiggingLastFrame = false;
	FVector PreviousLocation;
	
	// 찔르기 동작 완료 후 재시작을 위한 변수들
	bool bDigActionCompleted = false;
	float DigActionCooldown = 0.5f; // 찔르기 동작 완료 후 0.5초 대기
	float DigActionTimer = 0.0f;
    
	void UpdateDigSwingState(float DeltaTime);
	void UpdateDigPatternState(float DeltaTime);
	bool EvaluateStabbingMotion(const FVector& CurrentLocation, const FVector& Velocity);
	bool EvaluateLiftingMotion(const FVector& CurrentLocation, const FVector& Velocity);
	void ResetDigPattern();

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
