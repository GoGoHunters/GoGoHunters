#include "LHJ/CGlobe.h"
#include "GameFramework/Actor.h"
#include "Components/SceneComponent.h"


ACGlobe::ACGlobe()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ACGlobe::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACGlobe::Grab(USceneComponent* MotionController)
{
	if (!GrabbedController) return;
	
	bIsGrabbed = true;
	GrabbedController = MotionController;
	LastControllerQuat = GrabbedController->GetComponentRotation().Quaternion();
}

void ACGlobe::Release()
{
	bIsGrabbed = false;
	GrabbedController = nullptr;
}

void ACGlobe::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsGrabbed && GrabbedController)
		UpdateRotationFromController();
}

void ACGlobe::UpdateRotationFromController()
{
	// 컨트롤러의 현재 회전값
	FQuat CurrentQuat = GrabbedController->GetComponentQuat();
	// 이전 프레임의 컨트롤러 회전값
	FQuat LastQuat = LastControllerQuat;
	// 두 쿼터니언의 차이(회전 변화량)
	FQuat DeltaQuat = CurrentQuat * LastQuat.Inverse();

	// 쿼터니언 회전 적용
	AddActorLocalRotation(DeltaQuat);

	// 현재 회전값을 저장
	LastControllerQuat = GrabbedController->GetComponentRotation().Quaternion();
}

