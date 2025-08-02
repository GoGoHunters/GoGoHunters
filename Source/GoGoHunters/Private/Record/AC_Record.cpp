// Fill out your copyright notice in the Description page of Project Settings.


#include "Record/AC_Record.h"

#include "Kismet/GameplayStatics.h"
#include "Misc/FileHelper.h"
#include "HAL/FileManager.h"


// Sets default values for this component's properties
UAC_Record::UAC_Record()
{
    bAutoActivate = true;
	PrimaryComponentTick.bCanEverTick = false;
    bWantsInitializeComponent = true;

    AudioCapture = CreateDefaultSubobject<UAudioCaptureComponent>(TEXT("AudioCaptureComponent"));
    AudioCapture->bAutoActivate = false;
    AudioCapture->SetSubmixSend(nullptr, 0.0f);
}


void UAC_Record::InitializeComponent()
{
    Super::InitializeComponent();

}

// Called when the game starts
void UAC_Record::BeginPlay()
{
	Super::BeginPlay();
    
    InitializeOwnerPawn();
    InitializeAudioCapture();
    InitializeAudioCaptureComponent();
}

void UAC_Record::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (MicrophoneCapture && MicrophoneCapture->IsStreamOpen())
    {
        MicrophoneCapture->StopStream();
        MicrophoneCapture->CloseStream();
        bIsMicCapturing = false;
        UE_LOG(LogTemp, Log, TEXT("AC_Record: FAudioCapture stream stopped and closed."));
    }

    if (AudioCapture && AudioCapture->IsPlaying())
    {
        AudioCapture->Stop();
        UE_LOG(LogTemp, Log, TEXT("AC_Record: UAudioCaptureComponent stopped on EndPlay."));

        AudioCapture->OnAudioEnvelopeValue.RemoveDynamic(this, &UAC_Record::HandleAudioEnvelopeValue);
    }

    Super::EndPlay(EndPlayReason);
}


bool UAC_Record::InitializeOwnerPawn()
{
    AActor* OwningActor = GetOwner();

    OwnerPawn = Cast<APawn>(OwningActor);
    if (!OwnerPawn)
    {
        UE_LOG(LogTemp, Warning, TEXT(" Attached to non-Pawn Actor: %s"), *OwningActor->GetName());
        return false;
    }
    UE_LOG(LogTemp, Log, TEXT("Successfully attached to Pawn: %s"), *OwnerPawn->GetName());
    return true;
}

bool UAC_Record::InitializeAudioCapture()
{
    MicrophoneCapture = TUniquePtr<Audio::FAudioCapture>(new Audio::FAudioCapture());

    if (!MicrophoneCapture)
    {
        UE_LOG(LogTemp, Error, TEXT("AC_Record: Failed to open FAudioCapture stream."));
        return false;
    }

    // Default audio device
    Audio::FAudioCaptureDeviceParams DeviceParams;
    DeviceParams.DeviceIndex = INDEX_NONE;

    CurrentSampleRate = 48000;
    CurrentNumChannels = 1;

    // DeviceParams.SampleRate = CurrentSampleRate;
    // DeviceParams.NumChannels = CurrentNumChannels;

    TArray<Audio::FCaptureDeviceInfo> AvailableDevices;

    MicrophoneCapture->GetCaptureDevicesAvailable(AvailableDevices);

    if (AvailableDevices.Num() > 0)
    {
        // 장치들 목록 보기.
        UE_LOG(LogTemp, Log, TEXT("AC_Record: Found Audio Capture Devices:"));
        for (const Audio::FCaptureDeviceInfo& DeviceInfo : AvailableDevices)
        {
            UE_LOG(LogTemp, Log, TEXT("Device Name: %s"), *DeviceInfo.DeviceName);
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("AC_Record: No audio capture devices found. Using default SampleRate/Channels."));
    }

    Audio::FOnAudioCaptureFunction CaptureCallback;

    CaptureCallback = [this](const void* InAudio, int32 NumFrames, int32 NumChannels, int32 SampleRate, double StreamTime, bool bOverFlow)
    {
        this->OnMicrophoneAudioCaptured(InAudio, NumFrames, NumChannels, SampleRate, StreamTime, bOverFlow);
    };

    if (MicrophoneCapture->OpenAudioCaptureStream(DeviceParams, CaptureCallback, 2048))
    {
        if (MicrophoneCapture->StartStream())
        {
            bIsMicCapturing = true;
            CurrentSampleRate = MicrophoneCapture->GetSampleRate();
            CurrentNumChannels = DeviceParams.NumInputChannels; 
            UE_LOG(LogTemp, Log, TEXT("AC_Record: FAudioCapture stream started successfully. SampleRate: %d, Channels: %d"), CurrentSampleRate, CurrentNumChannels);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("AC_Record: Failed to start FAudioCapture stream."));
            MicrophoneCapture->CloseStream();
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("AC_Record: Failed to open FAudioCapture stream."));
    }
    return true;
}

bool UAC_Record::InitializeAudioCaptureComponent()
{
    if (!AudioCapture)
    {
        UE_LOG(LogTemp, Error, TEXT("AudioCapture component is NULL"));
        return false;
    }
    // 중요한 부분: 컴포넌트가 생성되었지만 엔진에 등록되지 않았을 수 있으므로 명시적으로 등록
    AudioCapture->RegisterComponent();

    if (!OwnerPawn || !OwnerPawn->GetRootComponent())
    {
        UE_LOG(LogTemp, Log, TEXT("Error Record Component Owner Not Found."));
        return false;
    }
    
    AudioCapture->AttachToComponent(OwnerPawn->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
    // AudioCapture->SetupAttachment(OwnerPawn->GetDefaultAttachComponent());

    AudioCapture->OnAudioEnvelopeValue.AddDynamic(this, &UAC_Record::HandleAudioEnvelopeValue);
    AudioCapture->Start();
    // StartAudioCapture(AudioCapture);
    UE_LOG(LogTemp, Log, TEXT("Record Audio Component Complete and Start"));
    return true;
}

void UAC_Record::OnMicrophoneAudioCaptured(const void* InAudio, int32 NumFrames, int32 NumChannels, int32 SampleRate, double StreamTime, bool bOverFlow)
{
    if (!isRecording)
        return;

    if (bOverFlow)
        UE_LOG(LogTemp, Warning, TEXT("AC_Record: Audio Capture Buffer Overflow!"));

    const float* AudioData = static_cast<const float*>(InAudio);

    int32 TotalSamples = NumFrames * NumChannels;

    TArray<float> CurrentCapturedData;
    CurrentCapturedData.SetNumUninitialized(TotalSamples);
    FMemory::Memcpy(CurrentCapturedData.GetData(), AudioData, TotalSamples * sizeof(float));

    CapturedPCMBuffer.Append(CurrentCapturedData);

    if (CapturedPCMBuffer.Num() >= SendBufferThresholdSamples)
        ProcessAndBroadcastCapturedData(SampleRate, NumChannels);
}

bool UAC_Record::ProcessAndBroadcastCapturedData(int32 SampleRateOverride, int32 NumChannelsOverride)
{
    if (CapturedPCMBuffer.Num() == 0)
        return false;

    TArray<uint8> ConvertedPCMBytes = ConvertFloatToPCM16BitBytes(CapturedPCMBuffer, NumChannelsOverride > 0 ? NumChannelsOverride : CurrentNumChannels);
    CapturedPCMBuffer.Empty();

    // WebSocket 전송을 위해 WAV 헤더를 붙여 완전한 WAV 데이터로 만듭니다.
    TArray<uint8> FullWavData;
    int32 ActualSampleRate = SampleRateOverride > 0 ? SampleRateOverride : CurrentSampleRate;
    int32 ActualNumChannels = NumChannelsOverride > 0 ? NumChannelsOverride : CurrentNumChannels;

    if (ActualSampleRate > 0 && ActualNumChannels > 0)
    {
        TArray<uint8> WavHeader = WavHeaderUtils::CreateWavHeader(ActualSampleRate, ActualNumChannels, 16, ConvertedPCMBytes.Num());
        FullWavData.Append(WavHeader);
    }
    FullWavData.Append(ConvertedPCMBytes);

    // BlueprintAssignable 델리게이트는 게임 스레드에서 호출해야 합니다.
    if (OnMicrophoneDataCaptured.IsBound())
    {
        AsyncTask(ENamedThreads::GameThread, [this, FullWavData]() {
                if (OnMicrophoneDataCaptured.IsBound())
                    OnMicrophoneDataCaptured.Broadcast(FullWavData);
        });
    }
    return true;
}

TArray<uint8> UAC_Record::ConvertFloatToPCM16BitBytes(const TArray<float>& FloatAudioData, int32 NumChannels)
{
    TArray<uint8> PCMBytes;
    PCMBytes.SetNumUninitialized(FloatAudioData.Num() * sizeof(int16));

    for (int32 i = 0; i < FloatAudioData.Num(); ++i)
    {
        // float 데이터를 -1.0 ~ 1.0 범위로 클램프하고 16비트 정수 범위(-32768 ~ 32767)로 스케일링
        int16 Sample = FMath::Clamp(FloatAudioData[i], -1.0f, 1.0f) * 32767.0f;
        // 16비트 정수를 바이트 배열에 복사 (Little-endian 방식)
        FMemory::Memcpy(PCMBytes.GetData() + (i * sizeof(int16)), &Sample, sizeof(int16));
    }
    return PCMBytes;
}


void UAC_Record::HandleAudioEnvelopeValue(const float Volume)
{
    if (debugRecord)
        UE_LOG(LogTemp, Log, TEXT("HandleAudioEnvelopeValue: Volume = %f"), Volume);
    if (Volume > minVolume)
    {
        startRecordTime = GetWorld()->GetTimeSeconds();
        if (isRecording)
            return;
        isRecording = true;
        OnRecordingStateChanged.Broadcast(true);

        // StartRecordFile();
        StartRecordingVoice();
    }
    else
    {
        if (!isRecording)
            return;
        if (GetWorld()->GetTimeSeconds() > startRecordTime + 2.0f)
        {
            //StopRecordFile();
            StopRecordingVoice();
            historyCount += 1;
            isRecording = false;
            OnRecordingStateChanged.Broadcast(false);
        }
    }
}

void UAC_Record::StartRecordFile()
{
    UAudioMixerBlueprintLibrary::StartRecordingOutput(this, 60.0f, OutputRecordingSubmix);
    UE_LOG(LogTemp, Display, TEXT("Record start"));

}

void UAC_Record::StopRecordFile()
{
    FString filename = FString::Printf(TEXT("record_%d"), historyCount);
    FString filepath = "Record";
    FString FileFullPath = FPaths::ProjectSavedDir() / TEXT("BouncedWavFiles") / filepath / filename + TEXT(".wav");

    UAudioMixerBlueprintLibrary::StopRecordingOutput(this, EAudioRecordingExportType::WavFile , filename, filepath, OutputRecordingSubmix, nullptr);

    UE_LOG(LogTemp, Display, TEXT("Record end"));

    OnRecordFileSavedEvent.Broadcast(FileFullPath);

}

void UAC_Record::StartRecordingVoice()
{
    if (isRecording)
        return;
    CapturedPCMBuffer.Empty();
    UE_LOG(LogTemp, Display, TEXT("AC_Record: Voice recording started (via FAudioCapture)."));
}

void UAC_Record::StopRecordingVoice()
{
    if (!isRecording)
        return;
    ProcessAndBroadcastCapturedData(CurrentSampleRate, CurrentNumChannels);
    
    EndRecordingVoice();
}

void UAC_Record::EndRecordingVoice()
{
    TArray<uint8> WavHeader = WavHeaderUtils::CreateWavHeader(0, 0, 0, 0);
    if (OnMicrophoneDataCaptured.IsBound())
    {
        AsyncTask(ENamedThreads::GameThread, [this, WavHeader]() {
            if (OnMicrophoneDataCaptured.IsBound())
                OnMicrophoneDataCaptured.Broadcast(WavHeader);
            });
    }
}
    
namespace WavHeaderUtils
{
    TArray<uint8> CreateWavHeader(int32 SampleRate, int32 NumChannels, int32 BitsPerSample, int32 PCMDataSize)
    {
        TArray<uint8> HeaderBytes;
        HeaderBytes.SetNumUninitialized(44); // Standard WAV header size

        // RIFF chunk
        FMemory::Memcpy(HeaderBytes.GetData() + 0, "RIFF", 4);
        int32 ChunkSize = 36 + PCMDataSize;
        FMemory::Memcpy(HeaderBytes.GetData() + 4, &ChunkSize, 4);
        FMemory::Memcpy(HeaderBytes.GetData() + 8, "WAVE", 4);

        // FMT sub-chunk
        FMemory::Memcpy(HeaderBytes.GetData() + 12, "fmt ", 4);
        int32 Subchunk1Size = 16; // PCM
        FMemory::Memcpy(HeaderBytes.GetData() + 16, &Subchunk1Size, 4);
        int16 AudioFormat = 1; // PCM
        FMemory::Memcpy(HeaderBytes.GetData() + 20, &AudioFormat, 2);
        int16 NumChan = NumChannels;
        FMemory::Memcpy(HeaderBytes.GetData() + 22, &NumChan, 2);
        FMemory::Memcpy(HeaderBytes.GetData() + 24, &SampleRate, 4);
        int32 ByteRate = SampleRate * NumChannels * (BitsPerSample / 8);
        FMemory::Memcpy(HeaderBytes.GetData() + 28, &ByteRate, 4);
        int16 BlockAlign = NumChannels * (BitsPerSample / 8);
        FMemory::Memcpy(HeaderBytes.GetData() + 32, &BlockAlign, 2);
        int16 Bps = BitsPerSample;
        FMemory::Memcpy(HeaderBytes.GetData() + 34, &Bps, 2);

        // DATA sub-chunk
        FMemory::Memcpy(HeaderBytes.GetData() + 36, "data", 4);
        FMemory::Memcpy(HeaderBytes.GetData() + 40, &PCMDataSize, 4);

        return HeaderBytes;
    }
}
