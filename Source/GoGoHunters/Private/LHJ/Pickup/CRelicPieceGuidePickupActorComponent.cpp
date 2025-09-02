#include "LHJ/Pickup/CRelicPieceGuidePickupActorComponent.h"

void UCRelicPieceGuidePickupActorComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
                                                          FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (FirstHandComponent)
		UpdateRotationFromController();
}

void UCRelicPieceGuidePickupActorComponent::GrabUsingForRelicPieceGuide()
{
	LastControllerQuat = FirstHandComponent->GetComponentRotation().Quaternion();
	bIsPulling = false;
}

void UCRelicPieceGuidePickupActorComponent::ReleaseUsingForRelicPieceGuide()
{
	PendingGrabComponent->SetSimulatePhysics(false);
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

	LastControllerQuat = FirstHandComponent->GetComponentRotation().Quaternion();
}
