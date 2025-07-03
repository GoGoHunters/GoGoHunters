// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_VRPlayer.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "IXRTrackingSystem.h"
#include "Camera/CameraComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "MotionControllerComponent.h"
#include "Utilities/CHelpers.h"
#include "Engine/OverlapResult.h"
#include "JMH/MH_GrabComp.h"
#include "JMH/MH_TeleportComp.h"
#include "LHM/Excavation/DetectorTool.h"
#include "LHJ/CWorldMap.h"
#include "EngineUtils.h"

// Sets default values
AMH_VRPlayer::AMH_VRPlayer()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	VRCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("VRCamera"));
	VRCamera->SetupAttachment(RootComponent);

	TeleportCircleA = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TeleportCircle"));
	TeleportCircleA->SetupAttachment(RootComponent);

	TeleportUIComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TeleportUIComponent"));
	TeleportUIComponent->SetupAttachment(RootComponent);

	CHelpers::CreateComponent<UMotionControllerComponent>(this, &RHandController, "RHandController", RootComponent);
	RHandController->SetTrackingMotionSource(FName("Right"));
	CHelpers::CreateComponent<UMotionControllerComponent>(this, &LHandController, "LHandController", RootComponent);
	LHandController->SetTrackingMotionSource(FName("Left"));
	//그랩 컴프
	GrabComponent = CreateDefaultSubobject<UMH_GrabComp>(TEXT("GrabComponent"));

	//Attachment 나중에 RootComp로 바꿔야함 /수정
	L_Hand = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("L_Hand"));
	L_Hand->SetupAttachment(VRCamera);
	R_Hand = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("R_Hand"));
	R_Hand->SetupAttachment(VRCamera);;

	LHandSKM = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LHandSKM"));
	LHandSKM->SetupAttachment(LHandController);
	LHandSKM->SetRelativeRotation(FRotator(-90.f, -90.f, 0.f));
	RHandSKM = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RHandSKM"));
	RHandSKM->SetupAttachment(RHandController);
	RHandSKM->SetRelativeRotation(FRotator(90.f, -90.f, 0.f));

	// 메시 로딩
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> LHandMeshAsset(TEXT("/Game/Characters/MannequinsXR/Meshes/SKM_MannyXR_left.SKM_MannyXR_left"));
	if (LHandMeshAsset.Succeeded())
	{
		LHandSKM->SetSkeletalMesh(LHandMeshAsset.Object);
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> RHandMeshAsset(TEXT("/Game/Characters/MannequinsXR/Meshes/SKM_MannyXR_right.SKM_MannyXR_right"));
	if (RHandMeshAsset.Succeeded())
	{
		RHandSKM->SetSkeletalMesh(RHandMeshAsset.Object);
	}

	//텔레포트 컴프
	TeleportComponent = CreateDefaultSubobject<UMH_TeleportComp>(TEXT("TeleportComponent"));
}

// Called when the game starts or when spawned
void AMH_VRPlayer::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InputMappingContext, 0);
		}
	}

	if (bUseVR)
	{
		GrabComponent->SetHandComponent(RHandController);
		TeleportComponent->SetHandComponent(RHandController);
	}
	else
	{
	// Test : VR 모드가 아니면 Scene Hand사용
		//Test 카메라 바라보는 방향으로 손 같이 움직이도록 손 VR 카메라에 Attach
		GrabComponent->SetHandComponent(R_Hand);
		TeleportComponent->SetHandComponent(R_Hand);

		RHandSKM->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		RHandSKM->UnregisterComponent();
		RHandSKM->SetupAttachment(R_Hand);
		RHandSKM->RegisterComponent();
		RHandSKM->SetRelativeRotation(FRotator(90.f, -90.f, 0.f));
		RHandSKM->SetRelativeLocation(FVector::ZeroVector);
		LHandSKM->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		LHandSKM->UnregisterComponent();
		LHandSKM->SetupAttachment(L_Hand);
		LHandSKM->RegisterComponent();
		LHandSKM->SetRelativeLocation(FVector::ZeroVector);
		LHandSKM->SetRelativeRotation(FRotator(-90.f, -90.f, 0.f));
	}
	TeleportComponent->SetTeleportVisual(TeleportCircleA, TeleportUIComponent);

	TeleportUIComponent->SetVisibility(false);
	TeleportCircleA->SetVisibility(false);

	// 월드에 존재하는 ACWorldMap을 찾아 저장
	for (TActorIterator<ACWorldMap> It(GetWorld()); It; ++It)
	{
		CachedWorldMap = *It;
		break;
	}
}

// Called every frame
void AMH_VRPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentState == EPlayerVRState::Teleporting)
	{
		UpdateInteractionLine();  // 이때만 라인 쏘기
	}
	else
	{
		// 라인 안 보일 땐 그랩도 안 되게 Actor를 비워두기
		FocusedGrabbableActor = nullptr;
	}
}

// Called to bind functionality to input
void AMH_VRPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	EnhancedInput->BindAction(IA_MHTurn, ETriggerEvent::Triggered, this, &AMH_VRPlayer::TestTurn);
	EnhancedInput->BindAction(IA_MHVRTurn, ETriggerEvent::Triggered, this, &AMH_VRPlayer::VRTurn);
	EnhancedInput->BindAction(IA_MHLookUp, ETriggerEvent::Triggered, this, &AMH_VRPlayer::TestLookUp);
	EnhancedInput->BindAction(IA_MHInteract, ETriggerEvent::Triggered, this, &AMH_VRPlayer::TriggerInteract);
	EnhancedInput->BindAction(IA_MHInteract, ETriggerEvent::Completed, this, &AMH_VRPlayer::TriggerInteractCompleted);
	EnhancedInput->BindAction(IA_MHInteract_L, ETriggerEvent::Triggered, this, &AMH_VRPlayer::TriggerInteract);
	EnhancedInput->BindAction(IA_MHInteract_L, ETriggerEvent::Completed, this, &AMH_VRPlayer::TriggerInteractCompleted);
	EnhancedInput->BindAction(IA_MHTestTeleportStart, ETriggerEvent::Started, this, &AMH_VRPlayer::F_TeleportStart);
	EnhancedInput->BindAction(IA_MHTestTeleportEnd, ETriggerEvent::Completed, this, &AMH_VRPlayer::F_TeleportEnd);
	EnhancedInput->BindAction(IA_MHVRTeleport, ETriggerEvent::Started, this, &AMH_VRPlayer::F_TeleportStart);
	EnhancedInput->BindAction(IA_MHVRTeleport, ETriggerEvent::Completed, this, &AMH_VRPlayer::F_TeleportEnd);

	//Grab
	EnhancedInput->BindAction(IA_MHGrab, ETriggerEvent::Started, this, &AMH_VRPlayer::TryGrab);
	EnhancedInput->BindAction(IA_MHGrab, ETriggerEvent::Completed, this, &AMH_VRPlayer::TryUnGrab);
	EnhancedInput->BindAction(IA_AdjustTeleportDirection, ETriggerEvent::Triggered, this,
	                          &AMH_VRPlayer::HandleThumbstickInput);
	EnhancedInput->BindAction(IA_RotateHeldObject, ETriggerEvent::Triggered, this,
	                          &AMH_VRPlayer::HandleThumbstickInput);

	// Excavation Tool Actions
	EnhancedInput->BindAction(IA_ExcavationTool1, ETriggerEvent::Triggered, this, &AMH_VRPlayer::ExcavationTool1);
	EnhancedInput->BindAction(IA_ExcavationTool2, ETriggerEvent::Triggered, this, &AMH_VRPlayer::ExcavationTool2);
	EnhancedInput->BindAction(IA_ExcavationTool3, ETriggerEvent::Triggered, this, &AMH_VRPlayer::ExcavationTool3);
}

void AMH_VRPlayer::SetPlayerState(EPlayerVRState NewState)
{
	if (CurrentState == NewState) return;

	// 상태 전환 로그
	UE_LOG(LogTemp, Log, TEXT("[VR] 상태 전환: %s → %s"),
	       *UEnum::GetValueAsString(CurrentState),
	       *UEnum::GetValueAsString(NewState));

	// 이전 상태 정리 (예: 도구 해제, 입력 정지 등 필요 시 여기에)

	// 상태 적용
	CurrentState = NewState;

	// 새 상태 진입 처리 (예: UI 안내, 모션 트리거 등 필요 시 여기에)
	switch (CurrentState)
	{
	case EPlayerVRState::Inspecting:
		// 예: UI에 "회전해서 살펴보세요!" 표시
		break;
	case EPlayerVRState::Excavating:
		// 예: 발굴 애니메이션 시작
		break;
	case EPlayerVRState::Disabled:
		DisableInput(nullptr);
		break;
	case EPlayerVRState::Idle:
		EnableInput(Cast<APlayerController>(GetController()));
		break;
	default:
		break;
	}
}

EPlayerVRState AMH_VRPlayer::GetPlayerState() const
{
	return CurrentState;
}

void AMH_VRPlayer::HandleThumbstickInput(const FInputActionValue& Value)
{
	FVector2D Input = Value.Get<FVector2D>();

	if (GrabComponent && GrabComponent->IsGrabbing())
	{
		// 물체 회전
		RotateHeldObject(Value);
	}
	else if (TeleportComponent && TeleportComponent->IsTeleporting())
	{
		AdjustTeleportDirection(Value);
	}
}

void AMH_VRPlayer::AdjustTeleportDirection(const FInputActionValue& Value)
{
	FVector2D Input = Value.Get<FVector2D>();

	TeleportDistanceFactor = FMath::Clamp(
		TeleportDistanceFactor + Input.Y * TeleportAdjustSpeed * GetWorld()->GetDeltaSeconds(),
		0.1f, 1.5f
	);
}

void AMH_VRPlayer::VRTurn(const FInputActionValue& Value)
{
	float AxisValue = Value.Get<float>();

	if (AxisValue > 0.5f)
	{
		AddControllerYawInput(SnapTurnAngle);
	}
	else if (AxisValue < -0.5f)
	{
		AddControllerYawInput(-SnapTurnAngle);
	}
}

void AMH_VRPlayer::F_TeleportStart(const struct FInputActionValue& Value)
{
	if (TeleportComponent)
	{
		TeleportComponent->EnableTeleport();
		SetPlayerState(EPlayerVRState::Teleporting);
	}
}

void AMH_VRPlayer::F_TeleportEnd(const struct FInputActionValue& Value)
{
	if (TeleportComponent)
	{
		FVector OutLocation;
		if (TeleportComponent->CompleteTeleport(OutLocation))
		{
			SetActorLocation(OutLocation);
			TeleportDistanceFactor = 1.0f;
		}
		SetPlayerState(EPlayerVRState::Idle);
	}
}

void AMH_VRPlayer::ActiveDebugDraw()
{
	bIsDebugDraw = !bIsDebugDraw;
}

void AMH_VRPlayer::RotateHeldObject(const struct FInputActionValue& Value)
{
	if (GrabComponent)
	{
		GrabComponent->RotateGrabbedObject(Value.Get<FVector2D>());
	}
}

void AMH_VRPlayer::TriggerInteract(const FInputActionInstance& IA_Instance)
{
	TryWorldMapInteraction(IA_Instance);
}

void AMH_VRPlayer::TriggerInteractCompleted()
{
	ResetWorldMapInteraction();
}

void AMH_VRPlayer::ExcavationTool1()
{
	if (!DetectionTool)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;

		if (DetectionToolClass = LoadClass<ADetectorTool>(nullptr, TEXT("/Game/LHM/BP/Excavation/BP_DetectorTool.BP_DetectorTool_C")))
		{
			DetectionTool = GetWorld()->SpawnActor<ADetectorTool>(DetectionToolClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
			if (DetectionTool)
			{
				USceneComponent* HandSocket = RHandController;
				DetectionTool->AttachToComponent(HandSocket, FAttachmentTransformRules::SnapToTargetNotIncludingScale, NAME_None);

				DetectionTool->StartDetection();

				//SetPlayerState(EPlayerVRState::Excavating);
			}
		}
	}
}

void AMH_VRPlayer::ExcavationTool2()
{

}

void AMH_VRPlayer::ExcavationTool3()
{

}

void AMH_VRPlayer::UpdateInteractionLine()
{
	if (CurrentState != EPlayerVRState::Idle && CurrentState != EPlayerVRState::Teleporting)
		return;

	FocusedGrabbableActor = nullptr;
	Lines.Empty();
	FVector Start;
	FVector Velocity;

	if (bUseVR)
	{
		Start = RHandController->GetComponentLocation();
		Velocity = RHandController->GetForwardVector() * 1000.f * TeleportDistanceFactor; // 강도는 상황에 맞게 조절
	}
	else
	{
		Start = R_Hand->GetComponentLocation();
		Velocity = R_Hand->GetForwardVector() * 1000.f * TeleportDistanceFactor; // 강도는 상황에 맞게 조절
	}

	FVector Pos = Start;
	Lines.Add(Pos);


	const float SimulateTime = 0.05f;
	const int LineSmooth = 30;
	const float GravityZ = -980.f;

	FVector LastPos = Pos;
	FVector FinalTeleportLocation = FVector::ZeroVector;
	bool bFoundTeleport = false;

	for (int32 i = 0; i < LineSmooth; ++i)
	{
		LastPos = Pos;
		Velocity += FVector(0.f, 0.f, GravityZ) * SimulateTime;
		Pos += Velocity * SimulateTime;

		FHitResult Hit;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);

		if (GetWorld()->LineTraceSingleByChannel(Hit, LastPos, Pos, ECC_Visibility, Params))
		{
			AActor* HitActor = Hit.GetActor();
			if (!HitActor) continue;

			// Grab 처리
			if (!FocusedGrabbableActor && HitActor->ActorHasTag("Grabbable"))
			{
				FocusedGrabbableActor = HitActor;
			}

			// Teleport 처리 (마지막 것만 저장)
			if (CurrentState == EPlayerVRState::Teleporting && HitActor->ActorHasTag("Teleportable"))
			{
				FinalTeleportLocation = Hit.Location;
				bFoundTeleport = true;
			}

			Pos = Hit.Location; // 라인을 히트 지점까지만 그리기
			Lines.Add(Pos);
			break;
		}

		Lines.Add(Pos);
	}

	if (TeleportUIComponent)
	{
		UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector(
			TeleportUIComponent, TEXT("User.PointArray"), Lines);
	}

	if (TeleportComponent)
	{
		if (bFoundTeleport)
		{
			TeleportComponent->UpdateTargetLocation(FinalTeleportLocation);
		}
		else if (CurrentState == EPlayerVRState::Teleporting)
		{
			TeleportComponent->SetInvalidTeleport();
		}
	}
}

void AMH_VRPlayer::TryGrab(const struct FInputActionValue& Value)
{
	if (!FocusedGrabbableActor) return;

	UPrimitiveComponent* HitComp = Cast<UPrimitiveComponent>(
		FocusedGrabbableActor->GetComponentByClass(UPrimitiveComponent::StaticClass()));
	if (!HitComp || !HitComp->IsSimulatingPhysics()) return;

	if (GrabComponent)
	{
		// 그랩 시도 → 성공하면 상태 전환
		if (GrabComponent->TryGrab(HitComp))
		{
			SetPlayerState(EPlayerVRState::GrabbingObject);
		}
	}
}

void AMH_VRPlayer::TryUnGrab(const struct FInputActionValue& Value)
{
	if (GrabComponent)
	{
		GrabComponent->TryUnGrab();
		SetPlayerState(EPlayerVRState::Idle);
	}
}

void AMH_VRPlayer::TestTurn(const FInputActionValue& Value)
{
	if (!bUseMouse)
	{
		return;
	}
	float AxisValue = Value.Get<float>();
	AddControllerYawInput(AxisValue);
}

void AMH_VRPlayer::TestLookUp(const FInputActionValue& Value)
{
	float AxisValue = Value.Get<float>();
	//AddControllerPitchInput(AxisValue);

	// VR 테스트 모드일 때만 적용 (예: HMD 미착용)
	if (!GEngine->XRSystem.IsValid() || !GEngine->XRSystem->IsHeadTrackingAllowed())
	{
		// 마우스 상하를 직접 VRCamera에 적용
		FRotator NewRot = VRCamera->GetRelativeRotation();
		NewRot.Pitch = FMath::Clamp(NewRot.Pitch + AxisValue, -80.f, 80.f);
		VRCamera->SetRelativeRotation(NewRot);
	}
}

void AMH_VRPlayer::TryWorldMapInteraction(const FInputActionInstance& IA_Instance)
{
	UMotionControllerComponent* MotionController = nullptr;
	if (IA_Instance.GetSourceAction() == IA_MHInteract) MotionController = RHandController;
	else if (IA_Instance.GetSourceAction() == IA_MHInteract_L) MotionController = LHandController;

	if (!MotionController) return;
	
	FVector Start = MotionController->GetComponentLocation();
	FVector End = Start + (MotionController->GetForwardVector() * 2000.f);

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 0.f, 0, 1);
	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_GameTraceChannel3, Params))
	{
		if (!HitResult.GetActor()->IsA(ACWorldMap::StaticClass())) return;
		UStaticMeshComponent* HitMesh = Cast<UStaticMeshComponent>(HitResult.GetComponent());
		if (HitMesh) Cast<ACWorldMap>(HitResult.GetActor())->EnableCompOutline(HitMesh);
	}
	else
	{
		ResetWorldMapInteraction();
	}
}

void AMH_VRPlayer::ResetWorldMapInteraction()
{
	if (CachedWorldMap)
	{
		CachedWorldMap->ResetPrevOutline();
	}
}
