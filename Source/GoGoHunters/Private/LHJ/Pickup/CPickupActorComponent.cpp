#include "LHJ/Pickup/CPickupActorComponent.h"

#include "MotionControllerComponent.h"
#include "JMH/MH_GrabComp.h"
#include "JMH/MH_VRPlayer.h"

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

		SetGrabActorScale(OwnerActor->GetActorRelativeScale3D());
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
			// // 회전
			// FVector FirstHandLocation = Player->RHandController->GetComponentLocation();
			// FVector SecondHandLocation = Player->LHandController->GetComponentLocation();
			// FRotator FindLookAtRotation = FRotationMatrix::MakeFromX(SecondHandLocation - FirstHandLocation).Rotator();
			//
			// FRotator FirstHandRotation = Player->RHandController->GetComponentRotation();
			//
			// FRotator MakeRotator(FindLookAtRotation.Pitch - 10, FindLookAtRotation.Yaw, FirstHandRotation.Roll);
			//
			// FQuat AQuat = FQuat(GrabRotation);
			// FQuat BQuat = FQuat(MakeRotator);
			// FRotator CombineRotators = FRotator(BQuat * AQuat);
			//
			// OwnerActor->SetActorRotation(CombineRotators);
			//===============================================
			// 크기
			FVector SecondHandPosition;
			if (SecondHandComponent==Player->LHandController)
				SecondHandPosition = Player->LHandController->GetComponentLocation();
			else
				SecondHandPosition = Player->RHandController->GetComponentLocation() * -1;
			
			FVector InverseTransformPosition = PendingGrabComponent->GetComponentTransform().InverseTransformPosition(
				SecondHandPosition);
			float SafeDivedX = (SecondHandAttachT.X != 0.0f)
				                   ? (InverseTransformPosition.X / SecondHandAttachT.X)
				                   : 0.0f;
			
			float ClampX = FMath::Clamp(SafeDivedX, MinScale3D.X, MaxScale3D.X);

			FVector ActorScale = OwnerActor->GetActorRelativeScale3D();

			float LerpX = FMath::Lerp(ActorScale.X, ClampX, 0.1f);

			
			OwnerActor->SetActorRelativeScale3D(FVector(LerpX, LerpX, LerpX));
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
		GrabUsingForRelicPiece();
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
			PendingGrabComponent->SetCollisionProfileName(OriginProfileName);
			PendingGrabComponent->SetCollisionResponseToChannels(GrabCollisionResponse);
			OwnerActor->SetActorScale3D(OriginScale3D);
			ReleaseUsingForRelic();
			ReleaseUsingForRelicPiece();
		}
	}
}

void UCPickupActorComponent::SetGrabActorScale(const FVector& Scale3D)
{
	OriginScale3D = Scale3D;
	MinScale3D = OriginScale3D * MinScalePercent;
	MaxScale3D = OriginScale3D * MaxScalePercent;
}

void UCPickupActorComponent::GrabUsingForRelic()
{
}

void UCPickupActorComponent::ReleaseUsingForRelic()
{
}

void UCPickupActorComponent::GrabUsingForRelicPiece()
{
}

void UCPickupActorComponent::ReleaseUsingForRelicPiece()
{
}
