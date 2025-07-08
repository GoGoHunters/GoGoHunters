// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AudioCaptureComponent.h"
#include "AudioMixerBlueprintLibrary.h"
#include "Sound/SoundSubmix.h"
#include "AC_Record.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRecordFileSaved, const FString&, FilePath);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class GOGOHUNTERS_API UAC_Record : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAC_Record();


private:
	bool InitializeOwnerPawn();
	bool InitializeAudioCaptureComponent();

	UFUNCTION()
	void HandleAudioEnvelopeValue(const float Volume);


	float minVolume = 0.05f;
	
	bool isRecording = false;
	
	double startRecordTime = 0.0f;
	
	int historyCount = 0;
	

protected:
	// Called when the game starts
	virtual void InitializeComponent() override;

	virtual void BeginPlay() override;


	UPROPERTY(BlueprintReadOnly, Category = "Owner")
	TObjectPtr<APawn> OwnerPawn;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AudioCapture")
	UAudioCaptureComponent* AudioCapture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AudioCapture")
	USoundSubmix* TargetAudioSubmix;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AudioRecording")
	USoundSubmix* OutputRecordingSubmix;

public :
	UFUNCTION(BlueprintImplementableEvent, Category = "AudioCapture")
	void StartAudioCapture(UAudioCaptureComponent* AudioCaptureComponent);

	UFUNCTION()
	void StartRecord();

	UFUNCTION()
	void StopRecord();

	UPROPERTY(BlueprintAssignable, Category = "AudioRecording")
	FOnRecordFileSaved OnRecordFileSavedEvent;

};
