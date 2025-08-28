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

	if (!PendingGrabComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("PendingGrabComponent is null"));
	}
	else
	{
		OriginProfileName = PendingGrabComponent->GetCollisionProfileName();
		GrabCollisionResponse = PendingGrabComponent->GetCollisionResponseToChannels();
	}

	OriginScale3D = PendingGrabComponent->GetRelativeScale3D();
	MinScale3D = OriginScale3D * MinScalePercent;
	MaxScale3D = OriginScale3D * MaxScalePercent;
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
			FVector FirstHandLocation = FirstHandComponent->GetComponentLocation();
			FVector SecondHandLocation = SecondHandComponent->GetComponentLocation();
			FRotator FindLookAtRotation = FRotationMatrix::MakeFromX(SecondHandLocation - FirstHandLocation).Rotator();

			FRotator FirstHandRotation = FirstHandComponent->GetComponentRotation();

			FRotator MakeRotator(FindLookAtRotation.Pitch - 10, FindLookAtRotation.Yaw, FirstHandRotation.Roll);

			FQuat AQuat = FQuat(GrabRotation);
			FQuat BQuat = FQuat(MakeRotator);
			FRotator CombineRotators = FRotator(BQuat * AQuat);

			OwnerActor->SetActorRotation(CombineRotators);

			//===============================================

			FVector InverseTransformPosition = PendingGrabComponent->GetComponentTransform().InverseTransformPosition(
				SecondHandComponent->GetComponentLocation());
			float SafeDivedeX = (SecondHandAttachT.X != 0.0f)
				                    ? (InverseTransformPosition.X / SecondHandAttachT.X)
				                    : 0.0f;
			float SafeDivedeY = (SecondHandAttachT.Y != 0.0f)
				                    ? (InverseTransformPosition.Y / SecondHandAttachT.Y)
				                    : 0.0f;
			float SafeDivedeZ = (SecondHandAttachT.Z != 0.0f)
				                    ? (InverseTransformPosition.Z / SecondHandAttachT.Z)
				                    : 0.0f;

			float ClampX = FMath::Clamp(SafeDivedeX, MinScale3D.X, MaxScale3D.X);
			float ClampY = FMath::Clamp(SafeDivedeY, MinScale3D.Y, MaxScale3D.Y);
			float ClampZ = FMath::Clamp(SafeDivedeZ, MinScale3D.Z, MaxScale3D.Z);

			FVector ActorScale = OwnerActor->GetActorRelativeScale3D();

			float LerpX = FMath::Lerp(ActorScale.X, ClampX, 0.2f);
			float LerpY = FMath::Lerp(ActorScale.Y, ClampY, 0.2f);
			float LerpZ = FMath::Lerp(ActorScale.Z, ClampZ, 0.2f);

			OwnerActor->SetActorRelativeScale3D(FVector(LerpX, LerpY, LerpZ));

			// FTransform ActorTransform = OwnerActor->GetActorTransform();
			// FRotator ActorInverseRotator = ActorTransform.GetRotation().Rotator().GetInverse();
			//
			// FVector InverseTransformPosition = FirstHandComponent->GetComponentTransform().InverseTransformPosition(SecondHandComponent->GetComponentLocation());
			//
			// float Dot = FVector::DotProduct(ActorTransform.GetLocation(), InverseTransformPosition);
			// float Selectflt = Dot < 0.f ? 1.f : -1.f;
			//
			// FVector Multi = InverseTransformPosition * Selectflt;
			// FRotator MakeFromZ = FRotationMatrix::MakeFromZ(Multi).Rotator();
			//
			// FQuat AQuat = FQuat(ActorInverseRotator);
			// FQuat BQuat = FQuat(MakeFromZ);
			// FRotator CombineRotators = FRotator(BQuat*AQuat);
			//
			// FVector RotateVector = CombineRotators.RotateVector(ActorTransform.GetLocation());
			// OwnerActor->SetActorLocationAndRotation(RotateVector, MakeFromZ);
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

			SecondHandAttachT = PendingGrabComponent->GetComponentTransform().InverseTransformPosition(
				SecondHandComponent->GetComponentLocation());
		}
		else
		{
			FirstHandComponent = AttachTo;
			PendingGrabComponent->SetSimulatePhysics(false);
			PendingGrabComponent->SetCollisionProfileName(GrabProfileName);
			bIsPulling = IsPulling;

			if (!bIsPulling)
				PendingGrabComponent->AttachToComponent(FirstHandComponent,
				                                        FAttachmentTransformRules::KeepWorldTransform);

			GrabUsingForRelic();
			Player->SetPlayerState(EPlayerVRState::GrabbingObject);

			GrabRotation = OwnerActor->GetActorRotation();
		}
	}
	// 한손 그랩 가능
	else
	{
		FirstHandComponent = AttachTo;
		PendingGrabComponent->SetSimulatePhysics(false);
		PendingGrabComponent->SetCollisionProfileName(GrabProfileName);
		// PendingGrabComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		bIsPulling = IsPulling;
		GrabUsingForRelic();
		Player->SetPlayerState(EPlayerVRState::GrabbingObject);
	}
}

void UCPickupActorComponent::Drop(USceneComponent* DropFrom)
{
	if (DropFrom == SecondHandComponent)
	{
		OwnerActor->SetActorRelativeRotation(FRotator::ZeroRotator);
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
			PendingGrabComponent->SetCollisionProfileName(OriginProfileName);
			PendingGrabComponent->SetCollisionResponseToChannels(GrabCollisionResponse);
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
