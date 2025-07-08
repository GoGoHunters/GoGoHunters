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

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	void OnStartFeedback();
	void UpdateFeedback(float Progress);
	void StopFeedback();

private:
	void PlayVibration(float Intensity);
	void UpdateVisualFeedback(float Progress);
	void PlaySoundFeedback(float Progress);

	UPROPERTY(EditAnywhere, Category = "Haptic")
	UHapticFeedbackEffect_Base* HapticEffect;
		
};
