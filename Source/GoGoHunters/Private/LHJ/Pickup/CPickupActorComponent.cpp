#include "LHJ/Pickup/CPickupActorComponent.h"

#include "JMH/MH_GrabComp.h"
#include "JMH/MH_VRPlayer.h"
#include "Kismet/KismetMathLibrary.h"

UCPickupActorComponent::UCPickupActorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCPickupActorComponent::BeginPlay()
{
	Super::BeginPlay();
	OwnerActor = GetOwner();
	if (OwnerActor)
	{
		TArray<UActorComponent*> tmpArr = OwnerActor->
			GetComponentsByTag(UPrimitiveComponent::StaticClass(), PickupName);
		if (tmpArr.Num() > 0)
			PendingGrabComponent = Cast<UPrimitiveComponent>(tmpArr[0]);
	}
}

void UCPickupActorComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                           FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

#pragma region Get Player Grab Component
	if (!bSetPlayerComp)
	{
		if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
		{
			if (APawn* Pawn = PC->GetPawn())
			{
				if (UActorComponent* comp = Pawn->GetComponentByClass(UMH_GrabComp::StaticClass()))
				{
					Player = Cast<AMH_VRPlayer>(Pawn);
					GrabComp = Cast<UMH_GrabComp>(comp);
					bSetPlayerComp = true;
				}
			}
		}
	}
#pragma endregion

	if (bIsPulling)
	{
		FVector Target = FirstHandComponent->GetComponentLocation(); //손 위치
		FVector Current = PendingGrabComponent->GetComponentLocation(); // 현재 위치
		FVector NewPos = FMath::VInterpTo(Current, Target, DeltaTime, GrabPullSpeed);
		PendingGrabComponent->SetWorldLocation(NewPos);

		//거의 손에 도달하면 붙이기
		if (FVector::Dist(NewPos, Target) < 10.f)
		{
			PendingGrabComponent->AttachToComponent(FirstHandComponent, FAttachmentTransformRules::KeepWorldTransform);
			bIsPulling = false;
		}
	}

	if (CanTwoHandGrab)
	{
		if (FirstHandComponent && SecondHandComponent)
		{
			FTransform ActorTransform = OwnerActor->GetActorTransform();
			FRotator ActorInverseRotator = ActorTransform.GetRotation().Rotator().GetInverse();

			FVector InverseTransformPosition = FirstHandComponent->GetComponentTransform().InverseTransformPosition(SecondHandComponent->GetComponentLocation());

			float Dot = FVector::DotProduct(ActorTransform.GetLocation(), InverseTransformPosition);
			float Selectflt = Dot < 0.f ? 1.f : -1.f;

			FVector Multi = InverseTransformPosition * Selectflt;
			FRotator MakeFromZ = FRotationMatrix::MakeFromZ(Multi).Rotator();
			
			FQuat AQuat = FQuat(ActorInverseRotator);
			FQuat BQuat = FQuat(MakeFromZ);
			FRotator CombineRotators = FRotator(BQuat*AQuat);

			FVector RotateVector = CombineRotators.RotateVector(ActorTransform.GetLocation());
			OwnerActor->SetActorLocationAndRotation(RotateVector, MakeFromZ);
		}
	}
}

void UCPickupActorComponent::Pickup(USceneComponent* AttachTo, bool IsPulling)
{
	if (!AttachTo) return;
	if (!PendingGrabComponent) return;

	// 양손 그랩 가능
	if (CanTwoHandGrab)
	{
		if (FirstHandComponent)
		{
			SecondHandComponent = AttachTo;
		}
		else
		{
			FirstHandComponent = AttachTo;
			PendingGrabComponent->SetSimulatePhysics(false);
			PendingGrabComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			bIsPulling = IsPulling;
			GrabUsingForRelic();
			Player->SetPlayerState(EPlayerVRState::GrabbingObject);
		}
	}
	// 한손 그랩 가능
	else
	{
		FirstHandComponent = AttachTo;
		PendingGrabComponent->SetSimulatePhysics(false);
		PendingGrabComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		bIsPulling = IsPulling;
		GrabUsingForRelic();
		Player->SetPlayerState(EPlayerVRState::GrabbingObject);
	}
}

void UCPickupActorComponent::Drop(USceneComponent* DropFrom)
{
	if (DropFrom == SecondHandComponent)
	{
		SecondHandComponent = nullptr;
	}
	else
	{
		PendingGrabComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		FirstHandComponent = nullptr;

		if (SecondHandComponent)
		{
			USceneComponent* NextAttachComponent = SecondHandComponent;
			SecondHandComponent = nullptr;
			Pickup(NextAttachComponent, false);
		}
		else
		{
			PendingGrabComponent->SetSimulatePhysics(true);
			PendingGrabComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			ReleaseUsingForRelic();
		}
	}
}

void UCPickupActorComponent::GrabUsingForRelic()
{
}

void UCPickupActorComponent::ReleaseUsingForRelic()
{
}
