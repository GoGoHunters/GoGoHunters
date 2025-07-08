// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/Excavation/DetectionComponent.h"
#include "Haptics/HapticFeedbackEffect_Base.h"

// Sets default values for this component's properties
UDetectionComponent::UDetectionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// 햅틱 이펙트
	static ConstructorHelpers::FObjectFinder<UHapticFeedbackEffect_Base> HapticAsset(TEXT("/Game/LHM/HapticFeedback/HFE_Detection.HFE_Detection"));
	if (HapticAsset.Succeeded())
	{
		HapticEffect = HapticAsset.Object;
	}
}


// Called when the game starts
void UDetectionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UDetectionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UDetectionComponent::OnStartFeedback()
{

}

void UDetectionComponent::UpdateFeedback(float Progress)
{
	float Normalized = Progress / 100.f;

	PlayVibration(Normalized);
	UpdateVisualFeedback(Normalized);
	PlaySoundFeedback(Normalized);
}

void UDetectionComponent::StopFeedback()
{
	// 햅틱 중지
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC)
	{
		PC->StopHapticEffect(EControllerHand::Right);
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
			//PC->PlayHapticEffect(HapticEffect, EControllerHand::Left, ClampedIntensity, false);
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
	// 예시: 가까워질수록 음량/피치 변화
	// SoundCue 또는 USoundBase 등 사용
	// UGameplayStatics::PlaySoundAtLocation(GetWorld(), SoundCue, GetOwner()->GetActorLocation(), ...);
}

