// Fill out your copyright notice in the Description page of Project Settings.


#include "Record/AC_Record.h"

// Sets default values for this component's properties
UAC_Record::UAC_Record()
{
    bAutoActivate = true;
	PrimaryComponentTick.bCanEverTick = false;
    bWantsInitializeComponent = true;

	AudioCapture = CreateDefaultSubobject<UAudioCaptureComponent>(TEXT("AudioCaptureComponent"));
	AudioCapture->bAutoActivate = false;
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
    InitializeAudioCaptureComponent();

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
    
    AudioCapture->OnAudioEnvelopeValue.AddDynamic(this, &UAC_Record::HandleAudioEnvelopeValue);
    StartAudioCapture(AudioCapture);
    UE_LOG(LogTemp, Log, TEXT("Record Audio Component Complete and Start"));
    return true;
}

void UAC_Record::HandleAudioEnvelopeValue(const float Volume)
{
    // UE_LOG(LogTemp, Log, TEXT("HandleAudioEnvelopeValue: Volume = %f"), Volume);
    if (Volume > minVolume)
    {
        startRecordTime = GetWorld()->GetTimeSeconds();
        if (isRecording)
            return;
        StartRecord();
    }
    else
    {
        if (!isRecording)
            return;
        if (GetWorld()->GetTimeSeconds() > startRecordTime + 2.0f)
        {
            StopRecord();
        }
    }
}

void UAC_Record::StartRecord()
{
    isRecording = true;
    UAudioMixerBlueprintLibrary::StartRecordingOutput(this, 60.0f, OutputRecordingSubmix);
    UE_LOG(LogTemp, Display, TEXT("Record start"));
}

void UAC_Record::StopRecord()
{
    FString filename = FString::Printf(TEXT("record_%d"), historyCount);
    FString filepath = "Record";
    FString FileFullPath = FPaths::ProjectSavedDir() / TEXT("BouncedWavFiles") / filepath / filename + TEXT(".wav");

    UAudioMixerBlueprintLibrary::StopRecordingOutput(this, EAudioRecordingExportType::WavFile , filename, filepath, OutputRecordingSubmix, nullptr);

    historyCount += 1;
    isRecording = false;

    UE_LOG(LogTemp, Display, TEXT("Record end"));

    OnRecordFileSavedEvent.Broadcast(FileFullPath);
} 