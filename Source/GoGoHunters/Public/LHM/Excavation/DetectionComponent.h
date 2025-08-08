// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DetectionComponent.generated.h"


UCLASS(/* ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) */)
class GOGOHUNTERS_API UDetectionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDetectionComponent();

public:
	void UpdateFeedback(float Progress);
	void StopFeedback();

private:
	void PlayVibration(float Intensity);
	void UpdateVisualFeedback(float Progress);
	void PlaySoundFeedback(float Progress);
	void PlayBeep();

	UPROPERTY(EditAnywhere, Category = "Effects")
	UHapticFeedbackEffect_Base* HapticEffect;
	
	UPROPERTY(EditAnywhere, Category = "Effects")
    class USoundBase* SoundEffect1;

	UPROPERTY(EditAnywhere, Category = "Effects")
    class USoundBase* SoundEffect2;

	FTimerHandle BeepTimerHandle;
	float CurrentBeepInterval = 1.0f; // 초기값
	float CurrentProgress = 0.f;

	float LastTimerUpdateTime = 0.f;
};
