 // Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/ActorComponent.h"
#include "AudioCaptureCore.h" 
#include "AudioCaptureDeviceInterface.h" 
#include "Misc/Paths.h" 
#include "Containers/Queue.h"

#include "AudioCaptureComponent.h"
#include "AudioMixerBlueprintLibrary.h"
#include "Sound/AudioBus.h"

#include "AC_Record.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRecordFileSaved, const FString&, FilePath);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMicrophoneDataCaptured, const TArray<uint8>&, CapturedAudioBytes);


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRecordingStateChanged, bool, bIsRecording);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRecordingEndedDelegate);


namespace WavHeaderUtils
{
	TArray<uint8> CreateWavHeader(int32 SampleRate, int32 NumChannels, int32 BitsPerSample, int32 PCMDataSize);
}

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class GOGOHUNTERS_API UAC_Record : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAC_Record();

private:
	TUniquePtr<Audio::FAudioCapture> MicrophoneCapture;

	bool InitializeOwnerPawn();
	bool InitializeAudioCapture();
	bool InitializeAudioCaptureComponent();

	bool bIsMicCapturing = false;
	void OnMicrophoneAudioCaptured(const void* InAudio, int32 NumFrames, int32 NumChannels, int32 SampleRate, double StreamTime, bool bOverFlow);
	TArray<uint8> ConvertFloatToPCM16BitBytes(const TArray<float>& FloatAudioData, int32 NumChannels);

	bool ProcessAndBroadcastCapturedData(int32 SampleRateOverride = 0, int32 NumChannelsOverride = 0);

	int32 CurrentSampleRate = 0; // 마이크의 실제 샘플 레이트
	int32 CurrentNumChannels = 0; // 마이크의 실제 채널 수

	TArray<float> CapturedPCMBuffer; // 캡처된 float 데이터를 임시로 저장할 버퍼

	float minVolume = 0.05f;
	bool isRecording = false; // 음성 감지에 따른 녹음 로직 (WebSocket 전송)
	double startRecordTime = 0.005f;

	int historyCount = 0;

	UPROPERTY(EditAnywhere, Category = "AudioRecording")
	int32 SendBufferThresholdSamples = 48000;

	UFUNCTION()
	void HandleAudioEnvelopeValue(const float Volume);

	UFUNCTION()
	void StartRecordingVoice();

	UFUNCTION()
	void StopRecordingVoice();

	UFUNCTION()
	void EndRecordingVoice();

	UFUNCTION()
	void StartRecordFile();

	UFUNCTION()
	void StopRecordFile();


protected:
	// Called when the game starts
	virtual void InitializeComponent() override;
	virtual void BeginPlay() override;;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(BlueprintReadOnly, Category = "Owner")
	TObjectPtr<APawn> OwnerPawn;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AudioCapture")
	TObjectPtr<UAudioCaptureComponent> AudioCapture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AudioRecording")
	USoundSubmix* OutputRecordingSubmix;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AudioCapture")
	bool debugRecord = false;
public :
	UFUNCTION(BlueprintImplementableEvent, Category = "AudioCapture")
	void StartAudioCapture(UAudioCaptureComponent* AudioCaptureComponent);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AudioCapture")
	bool isReadyToRecording = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AudioCapture")
	bool isWaitForServer = false;

	UPROPERTY(BlueprintAssignable, Category = "AudioRecording")
	FOnRecordFileSaved OnRecordFileSavedEvent;

	//UPROPERTY(BlueprintAssignable, Category = "AudioRecording")
	//FOnRecording OnRecordingEvent;
	UPROPERTY(BlueprintAssignable, Category = "AudioInput")
	FOnMicrophoneDataCaptured OnMicrophoneDataCaptured;


	UPROPERTY(BlueprintAssignable, Category = "AudioRecording")
	FOnRecordingStateChanged OnRecordingStateChanged; // 녹음 시작/중지 상태 변경 시 호출


	UPROPERTY(BlueprintAssignable, Category = "AudioRecording")
	FOnRecordingEndedDelegate OnRecordingEnded;
};

// https://dev.epicgames.com/documentation/en-us/unreal-engine/API/Runtime/Core/Misc 참조하기 최대 성능