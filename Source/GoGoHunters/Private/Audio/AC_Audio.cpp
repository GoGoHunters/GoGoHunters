// Fill out your copyright notice in the Description page of Project Settings.


#include "Audio/AC_Audio.h"
#include "Kismet/GameplayStatics.h"
#include "HAL/UnrealMemory.h"

// Sets default values for this component's properties
UAC_Audio::UAC_Audio()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
    Owner = GetOwner();

    bAutoActivate = false;
    SetVolumeMultiplier(1.0f);
    SetPitchMultiplier(1.0f);
}

void UAC_Audio::BeginPlay()
{
	ProceduralSoundWave = NewObject<USoundWaveProcedural>(this);
	if (ProceduralSoundWave)
	{
		// USoundWaveProcedural의 초기 오디오 포맷 속성 설정
		ProceduralSoundWave->SetSampleRate(DefaultSampleRate);
		ProceduralSoundWave->NumChannels = DefaultNumChannels;
		ProceduralSoundWave->Duration = INDEFINITELY_LOOPING_DURATION;
		ProceduralSoundWave->SoundGroup = ESoundGroup::SOUNDGROUP_Voice;
		ProceduralSoundWave->bLooping = false;
		ProceduralSoundWave->bProcedural = true;

		// UAudioComponent에 ProceduralSoundWave 할당
		SetSound(ProceduralSoundWave);
		UE_LOG(LogTemp, Log, TEXT("MyProceduralAudioComponent: USoundWaveProcedural initialized and assigned."));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("MyProceduralAudioComponent: Failed to create USoundWaveProcedural. Audio will not play."));
	}
}

void UAC_Audio::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (IsPlaying())
	{
		Stop();
	}

	// ProceduralSoundWave를 가비지 컬렉션 대상이 되도록 마크합니다.
	if (ProceduralSoundWave)
	{
		ProceduralSoundWave->MarkAsGarbage();
		ProceduralSoundWave = nullptr;
	}

	Super::EndPlay(EndPlayReason);
}

void UAC_Audio::PlayAudioFromBytes(const TArray<uint8>& AudioBytes)
{
	if (!ProceduralSoundWave)
	{
		UE_LOG(LogTemp, Error, TEXT("MyProceduralAudioComponent: ProceduralSoundWave not initialized. Cannot play audio."));
		return;
	}

	if (AudioBytes.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("MyProceduralAudioComponent: Received empty AudioBytes. No audio to play."));
		return;
	}
	const int32 WavHeaderSize = 44;

	if (AudioBytes.Num() < WavHeaderSize)
	{
		UE_LOG(LogTemp, Error, TEXT("MyProceduralAudioComponent: Received data too small to be a valid WAV file (expected at least %d bytes for header). Size: %d"), WavHeaderSize, AudioBytes.Num());
		return;
	}

	int32 ParsedSampleRate = 0;
	int16 ParsedNumChannels = 0;
	int16 ParsedBitsPerSample = 0;

	// fmt 청크 파싱 (RIFF, WAVE 헤더 이후부터 시작)
	int32 CurrentOffset = 12; // "RIFF" (4 bytes) + ChunkSize (4 bytes) + "WAVE" (4 bytes)

	// fmt 청크 찾기
	bool bFoundFmt = false;
	while (CurrentOffset < AudioBytes.Num() - 8) // 청크 ID(4) + 청크 사이즈(4) 최소 8바이트 필요
	{
		char ChunkID[5];
		FMemory::Memcpy(ChunkID, AudioBytes.GetData() + CurrentOffset, 4);
		ChunkID[4] = '\0'; // 널 종료 문자 추가

		int32 ChunkSize = 0;
		FMemory::Memcpy(&ChunkSize, AudioBytes.GetData() + CurrentOffset + 4, sizeof(int32));

		if (FString(ChunkID).Equals(TEXT("fmt "), ESearchCase::CaseSensitive))
		{
			// fmt 청크 발견!
			// 오디오 포맷 (20-21) -> 1 for PCM
			// NumChannels (22-23)
			// SampleRate (24-27)
			// BitsPerSample (34-35)
			FMemory::Memcpy(&ParsedNumChannels, AudioBytes.GetData() + CurrentOffset + 8 + 2, sizeof(int16)); // CurrentOffset + "fmt " (4) + ChunkSize (4) + AudioFormat (2)
			FMemory::Memcpy(&ParsedSampleRate, AudioBytes.GetData() + CurrentOffset + 8 + 4, sizeof(int32)); // CurrentOffset + "fmt " (4) + ChunkSize (4) + AudioFormat (2) + NumChannels (2)
			FMemory::Memcpy(&ParsedBitsPerSample, AudioBytes.GetData() + CurrentOffset + 8 + 14, sizeof(int16)); // CurrentOffset + "fmt " (4) + ChunkSize (4) + ... + BitsPerSample (2)
			bFoundFmt = true;
			break; // fmt 청크를 찾았으니 더 이상 탐색하지 않음
		}
		CurrentOffset += (8 + ChunkSize); // 다음 청크로 이동
	}

	if (!bFoundFmt)
	{
		UE_LOG(LogTemp, Error, TEXT("MyProceduralAudioComponent: 'fmt ' chunk not found in WAV file. Cannot play audio."));
		return;
	}
	UE_LOG(LogTemp, Log, TEXT("MyProceduralAudioComponent: Parsed WAV Header - SampleRate: %d, Channels: %d, BitsPerSample: %d"), ParsedSampleRate, ParsedNumChannels, ParsedBitsPerSample);

	ProceduralSoundWave->SetSampleRate(ParsedSampleRate);
	ProceduralSoundWave->NumChannels = ParsedNumChannels;

	UE_LOG(LogTemp, Log, TEXT("MyProceduralAudioComponent: Updated USoundWaveProcedural settings to SampleRate: %d, Channels: %d."), ParsedSampleRate, ParsedNumChannels);
	int32 DataChunkStartOffset = -1;
	CurrentOffset = 12; // 다시 RIFF 헤더 이후부터 시작 (fmt 청크를 건너뛰고 data를 찾아야 함)

	while (CurrentOffset < AudioBytes.Num() - 8)
	{
		char ChunkID[5];
		FMemory::Memcpy(ChunkID, AudioBytes.GetData() + CurrentOffset, 4);
		ChunkID[4] = '\0';

		int32 ChunkSize = 0;
		FMemory::Memcpy(&ChunkSize, AudioBytes.GetData() + CurrentOffset + 4, sizeof(int32));

		if (FString(ChunkID).Equals(TEXT("data"), ESearchCase::CaseSensitive)) // "data"는 공백 없음
		{
			DataChunkStartOffset = CurrentOffset + 8; // "data" (4 bytes) + ChunkSize (4 bytes) 이후가 실제 데이터 시작
			break;
		}
		CurrentOffset += (8 + ChunkSize);
	}

	if (DataChunkStartOffset == -1)
	{
		UE_LOG(LogTemp, Error, TEXT("MyProceduralAudioComponent: 'data' chunk not found in WAV file. Cannot play audio."));
		return;
	}

	const uint8* PCMDataPtr = AudioBytes.GetData() + DataChunkStartOffset;
	const int32 PCMDataSize = AudioBytes.Num() - DataChunkStartOffset;

	if (PCMDataSize > 0)
	{
		ProceduralSoundWave->QueueAudio(PCMDataPtr, PCMDataSize);
		UE_LOG(LogTemp, Log, TEXT("MyProceduralAudioComponent: Queued %d bytes of PCM data."), PCMDataSize);

		if (!IsPlaying())
		{
			Play();
			UE_LOG(LogTemp, Log, TEXT("MyProceduralAudioComponent: Started playing procedural sound from bytes."));
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("MyProceduralAudioComponent: Already playing, queuing more data."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("MyProceduralAudioComponent: No PCM data found to play after WAV header parsing."));
	}
}
