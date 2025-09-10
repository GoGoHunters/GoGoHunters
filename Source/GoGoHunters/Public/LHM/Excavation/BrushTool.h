// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BrushTool.generated.h"

UCLASS()
class GOGOHUNTERS_API ABrushTool : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABrushTool();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* Overlapped, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 BodyIndex, bool bFromSweep, const FHitResult& Hit);
	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* Overlapped, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 BodyIndex);

private:
	void CheckBrushSwipe(float DeltaTime);
	
public:
	void UpdateFeedback(float Intensity);
	void StopFeedback();

protected:
	void PlayVibration(float Intensity);
	void UpdateVisualFeedback(float Intensity);
	void PlaySoundFeedback(float Intensity);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Effects")
	class UHapticFeedbackEffect_Base* HapticEffect;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Effects")
	class UNiagaraSystem* BrushFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Effects")
    class USoundBase* SoundEffect;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USkeletalMeshComponent* BrushMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UBoxComponent* BoxMesh;

	UPROPERTY()
	class AActor* CurrentOverlappingRelic;

	UPROPERTY(BlueprintReadOnly)
	class ARelicsBase* Relic;

	// 최소/최대 스와이프 속도
	UPROPERTY(EditAnywhere)
	float BrushSwipeThresholdMin = 30.0f;
	UPROPERTY(EditAnywhere)
	float BrushSwipeThresholdMax = 200.0f;

	// 데칼 페이드 속도
	UPROPERTY(EditAnywhere)
	float FadeSpeed = 0.4f;

	FVector PreviousLocation;
	float SwipeSpeed = 0.0f;

	void SetIsBrushing(bool _bIsBrushing);
	bool bIsBrushing = false;

private:
	UPROPERTY(EditAnywhere, Category = "Warning|Feedback")
	USoundBase* WarningSFX = nullptr;

	UPROPERTY(EditAnywhere, Category = "Warning|Feedback")
	USoundBase* HardBrushSFX = nullptr;

	UPROPERTY(EditAnywhere, Category = "Warning|Feedback")
	UHapticFeedbackEffect_Base* HardBrushHaptic = nullptr;

	UPROPERTY(EditAnywhere, Category = "Warning")
	float WarningDelayAfterImpact = 2.0f; // 2초 지연

	// 경고 연출 + 지연 호출 래퍼
	void HandleBrushHardSwipeFeedbackAndWarn();

	// 2초 지연 후 판정 콜백
	void OnWarningDelayElapsed();

	// 진행 중인 경고 판정 취소
	void CancelPendingWarningCheck();

	void ResetWarningCooldown();

	FTimerHandle WarningCooldownHandle;
	FTimerHandle WarningDelayHandle;

	bool bCanTriggerWarning = true;
	float WarningCooldownDuration = 3.0f;

	// 2초 판정 상태
	bool bWarningCheckPending = false;
	bool bWarningWindowHadDropBelow = false;

// 사운드 재생 쿨타임
private:
	float LastSoundPlayTime = 0.0f;
	float SoundCooldown = 2.0f; // 최소 간격 1.0초 (사운드 길이)
	bool bIsSoundPlaying = false;
};
