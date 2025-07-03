// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/Excavation/DetectionComponent.h"

// Sets default values for this component's properties
UDetectionComponent::UDetectionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
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

}

void UDetectionComponent::PlayVibration(float Intensity)
{
	// VR 플랫폼별로 다름. 예시로 플레이어 컨트롤러에 햅틱 피드백 적용
	// (실제 PlayerController와 연결 필요)
	// 만약 오큘러스라면 Oculus Function Library 참고
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC)
	{
		// Left, Right 모두 진동. 강도(0.0 ~ 1.0)로 변환
		float ClampedIntensity = FMath::Clamp(Intensity, 0.0f, 1.0f);
		//PC->PlayHapticEffect(/* HapticEffectAsset */, EControllerHand::Left, ClampedIntensity);
		//PC->PlayHapticEffect(/* HapticEffectAsset */, EControllerHand::Right, ClampedIntensity);
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

