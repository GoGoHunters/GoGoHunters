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
			if (SecondHandComponent == Player->LHandController)
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

	//MH

	if (!Player)
	{
		if (AActor* HandOwner = AttachTo->GetOwner())
		{
			Player = Cast<AMH_VRPlayer>(HandOwner);
			if (!Player)
			{
				// 모션컨트롤러가 중간 컴포넌트일 수 있으므로 아우터 체인도 확인
				Player = HandOwner->GetTypedOuter<AMH_VRPlayer>();
			}
		}
	}

	// 안전하게: 못잡았으면 Player 쓰지 말자
	if (!Player)
	{
		UE_LOG(LogTemp, Warning, TEXT("Pickup: Player not resolved from AttachTo. Proceeding without Player-dependent logic."));
	}

	
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

/*
			// === 여기부터 추가: RestoreRelic 전용 소켓 스냅 ===
			if (OwnerActor && OwnerActor->ActorHasTag(FName("RestoreRelic")))
			{
				// 1) 손 스켈레탈 메쉬 찾기 (AttachTo는 보통 UMotionControllerComponent)
				auto* HandMesh = [&]()
				{
					if (auto* Skel = Cast<USkeletalMeshComponent>(FirstHandComponent)) return Skel;
					TArray<USceneComponent*> Children;
					FirstHandComponent->GetChildrenComponents(true, Children);
					for (USceneComponent* C : Children)
						if (auto* Skel = Cast<USkeletalMeshComponent>(C)) return Skel;
					return (USkeletalMeshComponent*)nullptr;
				}();

				// 2) 소켓명
				const bool bIsLeft = (Player && FirstHandComponent == Player->LHandController);
				const FName UseSocket = bIsLeft ? FName("L_Intex_Grab") : FName("R_Intex_Grab");

				// 3) 손 위치(소켓 기준)
				const FVector HandLoc = (HandMesh && UseSocket != NAME_None)
					                        ? HandMesh->GetSocketLocation(UseSocket)
					                        : FirstHandComponent->GetComponentLocation();

				// 4) 가장 가까운 GrabPoint 찾기 (태그=GrabPoint)
				auto* GrabPoint = [&]()
				{
					USceneComponent* Best = nullptr;
					float BestD2 = TNumericLimits<float>::Max();

					if (!OwnerActor || !OwnerActor->GetRootComponent()) return Best;

					TArray<USceneComponent*> Children;
					OwnerActor->GetRootComponent()->GetChildrenComponents(true, Children);
					for (USceneComponent* C : Children)
					{
						if (!C || !C->ComponentHasTag(FName("GrabPoint"))) continue;
						const float D2 = FVector::DistSquared(C->GetComponentLocation(), HandLoc);
						if (D2 < BestD2)
						{
							BestD2 = D2;
							Best = C;
						}
					}
					return Best ? Best : OwnerActor->GetRootComponent();
				}();

				// 5) GrabPoint 기준 오프셋 계산
				const FTransform RootWorld  = OwnerActor->GetRootComponent()->GetComponentTransform();
				const FTransform GrabWorld  = GrabPoint->GetComponentTransform();
				const FTransform RootRelToGrab = RootWorld.GetRelativeTransform(GrabWorld);

				const FTransform SocketWorld =
					(HandMesh && UseSocket != NAME_None)
						? HandMesh->GetSocketTransform(UseSocket, RTS_World)
						: FirstHandComponent->GetComponentTransform();

				// 원하는 루트 월드 = (Grab기준의 Root 오프셋) * (소켓의 월드)
				const FTransform NewRootWorld = RootRelToGrab * SocketWorld;

				// 6) 최종 적용: 월드 트랜스폼으로 직접 세팅 (스케일 보존)
				OwnerActor->SetActorTransform(NewRootWorld, false, nullptr, ETeleportType::TeleportPhysics);
			}
			else
			{
				// 기존 동작 유지: 끌어오기(false면 즉시 부착)
				if (!bIsPulling)
				{
					PendingGrabComponent->AttachToComponent(
						FirstHandComponent,
						FAttachmentTransformRules::KeepWorldTransform
					);
				}
			}
			// === 여기까지 추가 끝 ===
*/
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
//MH
/*
USceneComponent* UCPickupActorComponent::FindNearestGrabPoint(AActor* Piece, const FVector& HandLoc)
{
	if (!Piece || !Piece->GetRootComponent()) return nullptr;

	USceneComponent* Best = nullptr;
	float BestDistSqr = TNumericLimits<float>::Max();

	TArray<USceneComponent*> Children;
	Piece->GetRootComponent()->GetChildrenComponents(true, Children);

	for (USceneComponent* C : Children)
	{
		if (!C) continue;
		if (!C->ComponentHasTag(FName("GrabPoint"))) continue;

		const float D2 = FVector::DistSquared(C->GetComponentLocation(), HandLoc);
		if (D2 < BestDistSqr)
		{
			BestDistSqr = D2;
			Best = C;
		}
	}
	return Best;
}

USkeletalMeshComponent* UCPickupActorComponent::FindHandMesh(USceneComponent* AttachTo)
{
	if (!AttachTo) return nullptr;
	if (auto* Skel = Cast<USkeletalMeshComponent>(AttachTo)) return Skel;

	TArray<USceneComponent*> Children;
	AttachTo->GetChildrenComponents(true, Children);
	for (USceneComponent* C : Children)
		if (auto* Skel = Cast<USkeletalMeshComponent>(C))
			return Skel;

	return nullptr;
}
*/
