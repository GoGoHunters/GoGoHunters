// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DetectionComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GOGOHUNTERS_API UDetectionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDetectionComponent();

protected:
	virtual void BeginPlay() override;

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
    class USoundBase* DetectorSound1;

	UPROPERTY(EditAnywhere, Category = "Effects")
    class USoundBase* DetectorSound2;

public:
	UPROPERTY(EditAnywhere, Category = "Effects")
	FName VFXComponentTag = "DetectorVFX";
	
private:
    UPROPERTY(Transient)
    class UNiagaraComponent* VisualEffect = nullptr;

	FTimerHandle BeepTimerHandle;
	float CurrentBeepInterval = 1.0f; // 초기값
	float CurrentProgress = 0.f;

	float LastTimerUpdateTime = 0.f;

// 타미 대사
public:
	void PlayTami1();
	bool IsPlayingTami1() const { return bIsPlayingTami1; }

	void PlayTami2();
	bool IsPlayingTami2() const { return bIsPlayingTami2; }

private:
	bool bIsPlayingTami1 = false;
	bool bIsPlayingTami2 = false;
};
