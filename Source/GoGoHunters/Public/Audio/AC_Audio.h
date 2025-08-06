// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Sound/SoundWaveProcedural.h"
#include "Components/AudioComponent.h"
#include "AC_Audio.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlaybackEndedDelegate);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent), Blueprintable)
class GOGOHUNTERS_API UAC_Audio : public UAudioComponent
{
	GENERATED_BODY()

public:
	UAC_Audio();

private:
	UPROPERTY()
	USoundWaveProcedural* ProceduralSoundWave;
	UPROPERTY()
	UAudioComponent* AudioComponent;

	int32 DefaultSampleRate = 44100;
	int32 DefaultNumChannels = 2;
	int32 DefaultBitsPerSample = 16;

	AActor* Owner;


	// for make wav object cause check endpoint
	UPROPERTY()
	USoundWave* AssembledSoundWave;

	TArray<uint8> AssembledPCMData;

	int32 SampleRate = 0;
	int32 NumChannels = 0;
	int32 BitsPerSample = 0;


	// timer
	FTimerHandle StopAudioTimerHandle;
	UFUNCTION()
	void StopPlayback();

	UFUNCTION()
	void OnAudioPlaybackFinished();


protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	// Called when the component is removed from a game or destroyed
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


public:
	// play for live
	UFUNCTION(BlueprintCallable, Category = "AudioPlayback")
	void PlayAudioFromBytes(const TArray<uint8>& AudioBytes);


	UPROPERTY(BlueprintAssignable, Category = "AudioPlayback")
	FOnPlaybackEndedDelegate OnAudioPlaybackEnded;

	// make of wav object
	UFUNCTION(BlueprintCallable, Category = "AudioPlayback")
	void AddAudioDataChunk(const TArray<uint8>& AudioBytes, bool bIsLastChunk);

	UFUNCTION(BlueprintCallable, Category = "AudioPlayback")
	void PlayAssembledWav();
};