#include "JMH/MH_GrabComp.h"

#include "JMH/CMuseumComponent.h"
#include "JMH/MH_VRPlayer.h"
#include "LHJ/CRelicBase.h"

UMH_GrabComp::UMH_GrabComp()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UMH_GrabComp::BeginPlay()
{
	Super::BeginPlay();
	OwnerPlayer = Cast<AMH_VRPlayer>(GetOwner());
	if (OwnerPlayer) MuseumComponent = OwnerPlayer->GetComponentByClass<UCMuseumComponent>();
}

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
	if (MuseumComponent && MuseumComponent->GetMuseumState() != Display) return false;
	
	TargetComp->SetSimulatePhysics(false);
	TargetComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	PendingGrabComponent = TargetComp;
	bIsPulling = true;

	RelicGrab(TargetComp);

	return true;
}

void UMH_GrabComp::RelicGrab(UPrimitiveComponent* TargetComp)
{
	if (!MuseumComponent) return;
	if (!OwnerPlayer) return;
	if (!OwnerPlayer->GrabRelicActor) return;

	if (OwnerPlayer->GrabRelicActor->IsA(ACRelicBase::StaticClass()))
	{
		MuseumComponent->GrabRelic(Cast<ACRelicBase>(OwnerPlayer->GrabRelicActor));
	}
}

void UMH_GrabComp::TryUnGrab()
{
	if (!bIsGrabbing || !GrabbedComponent) return;
	if (MuseumComponent && MuseumComponent->GetMuseumState() != Display) return;

	ReleaseGrabbedComponent();
	RelicUnGrab();

	GrabbedComponent = nullptr;
	bIsGrabbing = false;

	UE_LOG(LogTemp, Warning, TEXT("[GrabComp] Grab 해제됨"));
}

void UMH_GrabComp::RelicUnGrab()
{
	if (!GrabbedComponent) return;
	if (!MuseumComponent) return;
	if (!OwnerPlayer) return;
	if (!OwnerPlayer->GrabRelicActor) return;
	
	if (OwnerPlayer->GrabRelicActor->IsA(ACRelicBase::StaticClass()))
	{
		MuseumComponent->GrabRelicEnd(Cast<ACRelicBase>(OwnerPlayer->GrabRelicActor), HandComponent->GetComponentLocation());
	}
	OwnerPlayer->GrabRelicActor = nullptr;
}

void UMH_GrabComp::ReleaseGrabbedComponent()
{
	GrabbedComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	GrabbedComponent->SetSimulatePhysics(true);
	GrabbedComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);	
}

void UMH_GrabComp::RotateGrabbedObject(const FVector2D& Input)
{
	if (!bIsGrabbing || !GrabbedComponent || Input.IsNearlyZero()) return;

	FRotator DeltaRot = FRotator::ZeroRotator;
	DeltaRot.Yaw += Input.X * HeldObjectRotateSpeed * GetWorld()->GetDeltaSeconds();
	DeltaRot.Pitch += Input.Y * HeldObjectRotateSpeed * GetWorld()->GetDeltaSeconds();

	GrabbedComponent->AddWorldRotation(DeltaRot);
}


