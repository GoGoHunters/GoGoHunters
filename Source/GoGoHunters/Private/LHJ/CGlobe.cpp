#include "LHJ/CGlobe.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"

#include "Utilities/CHelpers.h"


ACGlobe::ACGlobe()
{
	PrimaryActorTick.bCanEverTick = true;
	CHelpers::CreateComponent<USphereComponent>(this, &SphereComonent, "USphereComponent");
	CHelpers::CreateComponent<UStaticMeshComponent>(this, &Globe, "Globe", SphereComonent);

	this->SetActorScale3D(FVector(.6));
}

void ACGlobe::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACGlobe::Grab(USceneComponent* MotionController)
{
	if (!MotionController) return;
	
	bIsGrabbed = true;
	GrabbedController = MotionController;
	LastControllerQuat = GrabbedController->GetComponentRotation().Quaternion();
	AngularVelocityYaw = 0.f;
	bIsInertiaActive = false;
}

void ACGlobe::Release()
{
	bIsGrabbed = false;
	GrabbedController = nullptr;
	bIsInertiaActive = FMath::Abs(AngularVelocityYaw) > KINDA_SMALL_NUMBER;
}

void ACGlobe::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsGrabbed && GrabbedController)
	{
		UpdateRotationFromController();
	}
	else if (bIsInertiaActive)
	{
		// 관성 회전 적용 (Z축만)
		FRotator InertiaRot(0.f, AngularVelocityYaw * DeltaTime, 0.f);
		AddActorWorldRotation(InertiaRot, false, nullptr, ETeleportType::None);

		// 감쇠
		AngularVelocityYaw *= 0.95f;
		if (FMath::Abs(AngularVelocityYaw) < 0.01f)
		{
			bIsInertiaActive = false;
			AngularVelocityYaw = 0.f;
		}
	}
}

void ACGlobe::UpdateRotationFromController()
{
	FQuat CurrentQuat = GrabbedController->GetComponentQuat();
	FQuat LastQuat = LastControllerQuat;

	if ((CurrentQuat | LastQuat) < 0)
		CurrentQuat = CurrentQuat * -1.f;

	FQuat DeltaQuat = LastQuat * CurrentQuat.Inverse();
	FRotator DeltaRotator = DeltaQuat.Rotator();

	// Z축(Yaw)만 민감도 적용, 나머지 축은 0으로 고정
	float YawDelta = DeltaRotator.Yaw * RotationSensitivity;
	FRotator OnlyYawRotator(0.f, YawDelta, 0.f);
	FQuat OnlyYawQuat = FQuat(OnlyYawRotator);

	AddActorWorldRotation(OnlyYawQuat, false, nullptr, ETeleportType::None);

	// 각속도(Yaw) 저장 (초당 회전량)
	AngularVelocityYaw = YawDelta / FMath::Max(GetWorld()->GetDeltaSeconds(), KINDA_SMALL_NUMBER);

	LastControllerQuat = GrabbedController->GetComponentRotation().Quaternion();
}

