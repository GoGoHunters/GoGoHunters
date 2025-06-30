// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_GrabComp.h"

// Sets default values for this component's properties
UMH_GrabComp::UMH_GrabComp()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UMH_GrabComp::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UMH_GrabComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//손으로 물체 오는 중
	if (bIsPulling && IsValid(PendingGrabComponent) && HandComponent)
	{
		FVector Target = HandComponent->GetComponentLocation(); //손 위치
		FVector Current = PendingGrabComponent->GetComponentLocation(); // 현재 위치
		FVector NewPos = FMath::VInterpTo(Current, Target, DeltaTime, GrabPullSpeed);
		PendingGrabComponent->SetWorldLocation(NewPos);

		//거의 손에 도달하면 붙이기
		if (FVector::Dist(NewPos, Target) < 10.f)
		{
			PendingGrabComponent->AttachToComponent(HandComponent, FAttachmentTransformRules::KeepWorldTransform);
			GrabbedComponent = PendingGrabComponent;
			bIsGrabbing = true;
			PendingGrabComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			PendingGrabComponent = nullptr;
			bIsPulling = false;
			UE_LOG(LogTemp, Warning, TEXT("[GrabComp] Grab 완료"));
		}
	}
	else if (!IsValid(PendingGrabComponent))
	{
		// 잘못된 대상 초기화
		PendingGrabComponent = nullptr;
		bIsPulling = false;
	}
}

bool UMH_GrabComp::TryGrab(UPrimitiveComponent* TargetComp)
{
	if (!TargetComp) return false;

	TargetComp->SetSimulatePhysics(false);
	TargetComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PendingGrabComponent = TargetComp;
	bIsPulling = true;

	return true;
}

void UMH_GrabComp::TryUnGrab()
{
	if (!bIsGrabbing || !GrabbedComponent) return;

	GrabbedComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	GrabbedComponent->SetSimulatePhysics(true);
	GrabbedComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GrabbedComponent = nullptr;
	bIsGrabbing = false;

	UE_LOG(LogTemp, Warning, TEXT("[GrabComp] Grab 해제됨"));
}

void UMH_GrabComp::RotateGrabbedObject(const FVector2D& Input)
{
	if (!bIsGrabbing || !GrabbedComponent || Input.IsNearlyZero()) return;

	FRotator DeltaRot = FRotator::ZeroRotator;
	DeltaRot.Yaw += Input.X * HeldObjectRotateSpeed * GetWorld()->GetDeltaSeconds();
	DeltaRot.Pitch += Input.Y * HeldObjectRotateSpeed * GetWorld()->GetDeltaSeconds();

	GrabbedComponent->AddWorldRotation(DeltaRot);
}


