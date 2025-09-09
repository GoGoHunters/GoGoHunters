#include "LHJ/Pickup/CRelicPieceGuidePickupActorComponent.h"

UCRelicPieceGuidePickupActorComponent::UCRelicPieceGuidePickupActorComponent()
{
	bCanAttach = false;
}

void UCRelicPieceGuidePickupActorComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
                                                          FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (FirstHandComponent)
		UpdateRotationFromController();
	else if (bIsInertiaActive)
	{
		// 관성 회전 적용 (Z축만)
		FRotator InertiaRot(0.f, AngularVelocityYaw * DeltaTime, 0.f);
		OwnerActor->AddActorWorldRotation(InertiaRot, false, nullptr, ETeleportType::None);

		// 감쇠
		AngularVelocityYaw *= 0.95f;
		if (FMath::Abs(AngularVelocityYaw) < 0.01f)
		{
			bIsInertiaActive = false;
			AngularVelocityYaw = 0.f;
		}
	}
}

void UCRelicPieceGuidePickupActorComponent::GrabOverrideFunc()
{
	LastControllerQuat = FirstHandComponent->GetComponentRotation().Quaternion();
	bIsPulling = false;
	AngularVelocityYaw = 0.f;
	bIsInertiaActive = false;
}

void UCRelicPieceGuidePickupActorComponent::ReleaseOverrideFunc()
{
	PendingGrabComponent->SetSimulatePhysics(false);
	bIsInertiaActive = FMath::Abs(AngularVelocityYaw) > KINDA_SMALL_NUMBER;
}

void UCRelicPieceGuidePickupActorComponent::UpdateRotationFromController()
{
	FQuat CurrentQuat = FirstHandComponent->GetComponentQuat();
	FQuat LastQuat = LastControllerQuat;

	if ((CurrentQuat | LastQuat) < 0)
		CurrentQuat = CurrentQuat * -1.f;

	FQuat DeltaQuat = LastQuat * CurrentQuat.Inverse();
	FRotator DeltaRotator = DeltaQuat.Rotator();

	// Z축(Yaw)만 민감도 적용, 나머지 축은 0으로 고정
	float YawDelta = DeltaRotator.Yaw * RotationSensitivity;
	FRotator OnlyYawRotator(0.f, YawDelta, 0.f);
	FQuat OnlyYawQuat = FQuat(OnlyYawRotator);

	OwnerActor->AddActorWorldRotation(OnlyYawQuat, false, nullptr, ETeleportType::None);

	AngularVelocityYaw = YawDelta / FMath::Max(GetWorld()->GetDeltaSeconds(), KINDA_SMALL_NUMBER);

	LastControllerQuat = FirstHandComponent->GetComponentRotation().Quaternion();
}
