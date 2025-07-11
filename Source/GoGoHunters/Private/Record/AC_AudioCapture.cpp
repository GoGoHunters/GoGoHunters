#include "Record/AC_AudioCapture.h"
#include "Engine/Engine.h"

UAC_AudioCapture::UAC_AudioCapture()
{
    PrimaryComponentTick.bCanEverTick = false;
    CurrentVolumeLevel = 0.0f;
}

void UAC_AudioCapture::BeginPlay()
{
    Super::BeginPlay();

    // 오디오 캡처 객체 생성
    AudioCapture = MakeUnique<Audio::FAudioCapture>();

    // 캡처 파라미터 설정 (더 호환성 있는 설정)
    CaptureParams.bUseHardwareAEC = false;
    CaptureParams.DeviceIndex = INDEX_NONE; // 기본 마이크 디바이스 사용
    CaptureParams.NumInputChannels = 2; // 스테레오로 시도 (더 호환성 좋음)
    CaptureParams.SampleRate = 48000; // 48kHz로 변경 (더 널리 지원됨)
    CaptureParams.PCMAudioEncoding = Audio::EPCMAudioEncoding::PCM_16;
}

void UAC_AudioCapture::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    StopCapture();
    Super::EndPlay(EndPlayReason);
}

void UAC_AudioCapture::StartCapture()
{
    StartCaptureWithFallback();
}

void UAC_AudioCapture::StartCaptureWithFallback()
{
    if (!AudioCapture.IsValid())
        return;

    // 여러 설정을 시도해보는 배열
    struct AudioConfig
    {
        int32 SampleRate;
        int32 NumChannels;
        Audio::EPCMAudioEncoding Encoding;
    };

    TArray<AudioConfig> ConfigsToTry = {
        {48000, 2, Audio::EPCMAudioEncoding::PCM_16}, // 48kHz 스테레오 16-bit
        {44100, 2, Audio::EPCMAudioEncoding::PCM_16}, // 44.1kHz 스테레오 16-bit
        {48000, 1, Audio::EPCMAudioEncoding::PCM_16}, // 48kHz 모노 16-bit
        {44100, 1, Audio::EPCMAudioEncoding::PCM_16}, // 44.1kHz 모노 16-bit
        {22050, 1, Audio::EPCMAudioEncoding::PCM_16}, // 22kHz 모노 16-bit (최후 수단)
    };

    for (const auto& Config : ConfigsToTry)
    {
        CaptureParams.SampleRate = Config.SampleRate;
        CaptureParams.NumInputChannels = Config.NumChannels;
        CaptureParams.PCMAudioEncoding = Config.Encoding;

        UE_LOG(LogTemp, Log, TEXT("Trying audio config: %dHz, %d channels, %s"),
            Config.SampleRate, Config.NumChannels,
            Config.Encoding == Audio::EPCMAudioEncoding::PCM_16 ? TEXT("16-bit") : TEXT("Other"));

        if (TryStartCapture())
        {
            UE_LOG(LogTemp, Log, TEXT("Audio capture started successfully with config: %dHz, %d channels"),
                Config.SampleRate, Config.NumChannels);
            return;
        }
    }

    UE_LOG(LogTemp, Error, TEXT("Failed to start audio capture with any configuration"));
}

bool UAC_AudioCapture::TryStartCapture()
{
    if (!AudioCapture.IsValid())
        return false;

    Audio::FOnCaptureFunction AudioCallback = [this](const float* AudioData, int32 NumFrames, int32 NumChannels, int32 SampleRate, double StreamTime, bool bOverflow)
        {
            ProcessAudioData(AudioData, NumFrames, NumChannels, SampleRate, StreamTime, bOverflow);
        };

    uint32 NumFramesDesired = 10240;

    if (AudioCapture->OpenCaptureStream(CaptureParams, UAC_AudioCapture::ProcessAudioData, NumFramesDesired))
    {
        AudioCapture->StartStream();
        return true;
    }
    return false;
}

void UAC_AudioCapture::StopCapture()
{
    if (AudioCapture.IsValid())
    {
        AudioCapture->StopStream();
        AudioCapture->CloseStream();
        UE_LOG(LogTemp, Log, TEXT("Audio capture stopped"));
    }
}

void UAC_AudioCapture::ProcessAudioData(const float* AudioData, int32 NumFrames, int32 NumChannels, int32 SampleRate, double StreamTime, bool bOverflow)
{
    UE_LOG(LogTemp, Log, TEXT("Audio Level: %f, Frames: %d, Channels: %d, SampleRate: %d StreamTime : %f"), *AudioData, NumFrames, NumChannels, SampleRate, StreamTime);


    if (AudioData && NumFrames > 0)
    {
        // 볼륨 레벨 계산
        CalculateVolumeLevel(AudioData, NumFrames * NumChannels);

        // 오디오 데이터가 잘 들어오는지 로그로 확인
        if (CurrentVolumeLevel > 0.01f) // 임계값 이상일 때만 로그
        {
            UE_LOG(LogTemp, Log, TEXT("Audio Level: %f, Frames: %d, Channels: %d, SampleRate: %d"),
                CurrentVolumeLevel, NumFrames, NumChannels, SampleRate);
        }

        // 오버플로우 체크
        if (bOverflow)
        {
            UE_LOG(LogTemp, Warning, TEXT("Audio buffer overflow detected"));
        }
    }
}

void UAC_AudioCapture::CalculateVolumeLevel(const float* AudioData, int32 NumSamples)
{
    if (!AudioData || NumSamples <= 0)
    {
        CurrentVolumeLevel = 0.0f;
        return;
    }

    // RMS (Root Mean Square) 계산
    float Sum = 0.0f;
    for (int32 i = 0; i < NumSamples; ++i)
    {
        Sum += AudioData[i] * AudioData[i];
    }

    CurrentVolumeLevel = FMath::Sqrt(Sum / NumSamples);
}

float UAC_AudioCapture::GetCurrentVolumeLevel() const
{
    return CurrentVolumeLevel;
}