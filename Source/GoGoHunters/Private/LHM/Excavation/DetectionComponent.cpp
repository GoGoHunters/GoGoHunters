// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/Excavation/DetectionComponent.h"
#include "Haptics/HapticFeedbackEffect_Base.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"

// Sets default values for this component's properties
UDetectionComponent::UDetectionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// 햅틱 이펙트
	static ConstructorHelpers::FObjectFinder<UHapticFeedbackEffect_Base> HapticAsset(TEXT("/Game/LHM/Effects/HapticFeedback/HFE_Detection.HFE_Detection"));
	if (HapticAsset.Succeeded())
	{
		HapticEffect = HapticAsset.Object;
	}

	// 사운드 이펙트
	static ConstructorHelpers::FObjectFinder<USoundBase> Sound1Asset(TEXT("/Game/LHM/Effects/Sound/Excavation/SW_Detector_1.SW_Detector_1"));
	if(Sound1Asset.Succeeded())
	{
		DetectorSound1 = Sound1Asset.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundBase> Sound2Asset(TEXT("/Game/LHM/Effects/Sound/Excavation/SW_Detector_2.SW_Detector_2"));
	if (Sound2Asset.Succeeded())
	{
		DetectorSound2 = Sound2Asset.Object;
	}
}

void UDetectionComponent::UpdateFeedback(float Progress)
{
	CurrentProgress = Progress;
	float Normalized = Progress / 100.f;

	PlayVibration(Normalized);
	UpdateVisualFeedback(Normalized);
	PlaySoundFeedback(Progress);
}

void UDetectionComponent::StopFeedback()
{
	// 햅틱 중지
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC)
	{
		PC->StopHapticEffect(EControllerHand::Right);
	}

	// 타이머 초기화
	if (GetOwner())
	{
		GetOwner()->GetWorldTimerManager().ClearTimer(BeepTimerHandle);
		UE_LOG(LogTemp, Warning, TEXT("[DetectionComponent] Timer stopped in StopFeedback()"));
	}
}

void UDetectionComponent::PlayVibration(float Intensity)
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC)
	{
		float ClampedIntensity = FMath::Clamp(Intensity, 0.0f, 1.0f);

		if (HapticEffect)
		{
			PC->PlayHapticEffect(HapticEffect, EControllerHand::Right, ClampedIntensity, false);
		}
	}
}

void UDetectionComponent::UpdateVisualFeedback(float Progress)
{
	// 예시: Mesh, Light, Material, Particle 등 시각적 효과 업데이트
	// 액터의 머티리얼 컬러, 광도 등 변경
}

void UDetectionComponent::PlaySoundFeedback(float Progress)
{
	if (!DetectorSound1 || !GetOwner()) return;

	UWorld* World = GetWorld();
	if (!World) return;

	// 1. 탐지율 너무 낮으면 사운드 정지
	if (Progress < 10.f)
	{
		if (World->GetTimerManager().IsTimerActive(BeepTimerHandle))
		{
			World->GetTimerManager().ClearTimer(BeepTimerHandle);
			UE_LOG(LogTemp, Warning, TEXT("[DetectionComponent] Progress < 10 → Timer cleared"));
		}
		return;
	}

	//// 2. Progress가 90 이상이면 한 번만 재생하고 끝
	//if (Progress >= 90.0f)
	//{
	//	if (World->GetTimerManager().IsTimerActive(BeepTimerHandle))
	//	{
	//		PlayBeep();
	//	}
	//	return;
	//}

	// 2. 간격 계산
	float NewInterval = FMath::Lerp(1.0f, 0.01f, Progress / 100.f);

	// 3. 간격 변화가 0.05초 이상일 때만 타이머 재설정
	if (FMath::Abs(CurrentBeepInterval - NewInterval) > 0.05f || !World->GetTimerManager().IsTimerActive(BeepTimerHandle))
	{
		CurrentBeepInterval = NewInterval;

		// 기존 타이머 정리
		World->GetTimerManager().ClearTimer(BeepTimerHandle);

		// 새 타이머 설정
		FTimerDelegate BeepDelegate = FTimerDelegate::CreateUObject(this, &UDetectionComponent::PlayBeep);
		World->GetTimerManager().SetTimer(BeepTimerHandle, BeepDelegate, CurrentBeepInterval, true, 0.f);

		//UE_LOG(LogTemp, Warning, TEXT("[DetectionComponent] Timer updated: Interval = %.2f, Progress = %.2f"), CurrentBeepInterval, Progress);
	}
}

void UDetectionComponent::PlayBeep()
{
	if (!DetectorSound1 || !DetectorSound2 || !GetOwner()) return;
	//UE_LOG(LogTemp, Log, TEXT("[DetectionComponent] PlayBeep - Progress: %.2f, Interval: %.2f"), CurrentProgress, CurrentBeepInterval);
	
	/*if (CurrentProgress >= 90.0f)
	{
		UWorld* World = GetWorld();
		if (World && World->GetTimerManager().IsTimerActive(BeepTimerHandle))
		{
			World->GetTimerManager().ClearTimer(BeepTimerHandle);
			UE_LOG(LogTemp, Warning, TEXT("[DetectionComponent] Final detection reached - Timer stopped, playing long beep"));
		}

		UGameplayStatics::PlaySoundAtLocation(this, SoundEffect2, GetOwner()->GetActorLocation(), 1.0f);
		return;
	}*/

	const float Volume = FMath::Lerp(0.3f, 1.0f, CurrentProgress / 100.f);
	UGameplayStatics::PlaySoundAtLocation(this, DetectorSound1, GetOwner()->GetActorLocation(), Volume);
}

void UDetectionComponent::PlayTami1()
{
	if (!GetWorld()->GetMapName().ToLower().Contains("Excavation")) return; // 발굴 레벨에서만 실행

	bIsPlayingTami1 = true;

	APawn* TamiAI = nullptr;

	for (TActorIterator<APawn> It(GetWorld(), APawn::StaticClass()); It; ++It)
	{
		if (IsValid(*It) && (*It)->ActorHasTag(FName("Tami")))
		{
			TamiAI = *It;
			break;
		}
	}

	if (!TamiAI) return;

	FName FunctionName(TEXT("PlayExcavationPhase1_DetectorPercent"));
	if (UFunction* Function = TamiAI->FindFunction(FunctionName))
	{
		TamiAI->ProcessEvent(Function, nullptr);
	}
}

void UDetectionComponent::PlayTami2()
{
	bIsPlayingTami2 = true;

	APawn* TamiAI = nullptr;

	for (TActorIterator<APawn> It(GetWorld(), APawn::StaticClass()); It; ++It)
	{
		if (IsValid(*It) && (*It)->ActorHasTag(FName("Tami")))
		{
			TamiAI = *It;
			break;
		}
	}

	if (!TamiAI) return;

	FName FunctionName(TEXT("PlayExcavationPhase1_DetectorPercent100"));
	if (UFunction* Function = TamiAI->FindFunction(FunctionName))
	{
		TamiAI->ProcessEvent(Function, nullptr);
	}
}

