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

private:
	void DropPickedRelic();

	UFUNCTION()
    void OnRelicHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

    // 바운스 연속 히트 스팸 방지
    TMap<TWeakObjectPtr<UPrimitiveComponent>, float> LastHitTime;
    float ImpactCooldown = 1.0f; // 초
    float ImpactSpeedThreshold = 50.0f; // (드롭 경고와 동일/별도)

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Effects")
    class USoundBase* SoundFX;

private:
	UPROPERTY()
	class ARelicsBase* PickedRelic;

	UPROPERTY()
	class ARelicsBase* RelicCandidate = nullptr;

	UPROPERTY()
	class UStaticMeshComponent* CandidateMesh = nullptr;

// 타미 대사
private:
	void PlayTami();
	bool bIsPlayingTami = false;
};
