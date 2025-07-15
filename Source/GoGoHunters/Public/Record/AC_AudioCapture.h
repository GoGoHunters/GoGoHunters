// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AudioCapture.h"
#include "AudioCaptureCore.h"
#include "AC_AudioCapture.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GOGOHUNTERS_API UAC_AudioCapture : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAC_AudioCapture();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    // 오디오 캡처 객체
    TUniquePtr<Audio::FAudioCapture> AudioCapture;

    // 오디오 데이터 버퍼
    TArray<float> AudioBuffer;

    // 캡처 설정
    Audio::FAudioCaptureDeviceParams CaptureParams;

    // 오디오 데이터 처리 함수
    void ProcessAudioData(const float* AudioData, int32 NumFrames, int32 NumChannels, int32 SampleRate, double StreamTime, bool bOverflow);

public:
    // 여러 설정으로 시도하는 안전한 캡처 시작
    UFUNCTION(BlueprintCallable, Category = "Audio Capture")
    void StartCaptureWithFallback();

    // 캡처 시작/정지 함수
    UFUNCTION(BlueprintCallable, Category = "Audio Capture")
    void StartCapture();

    UFUNCTION(BlueprintCallable, Category = "Audio Capture")
    void StopCapture();

    // 현재 볼륨 레벨 가져오기
    UFUNCTION(BlueprintCallable, Category = "Audio Capture")
    float GetCurrentVolumeLevel() const;

private:
    // 현재 볼륨 레벨 (RMS 값)
    float CurrentVolumeLevel;

    // 볼륨 레벨 계산
    void CalculateVolumeLevel(const float* AudioData, int32 NumSamples);

    // 캡처 시작 시도 (내부 함수)
    bool TryStartCapture();
};
