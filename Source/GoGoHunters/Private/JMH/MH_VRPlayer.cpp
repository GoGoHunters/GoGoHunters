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
#include "LHM/Excavation/ShovelTool.h"
#include "LHJ/CWorldMap.h"
#include "EngineUtils.h"
#include "Components/WidgetInteractionComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "JMH/CMuseumComponent.h"
#include "LHJ/CRelicBase.h"
#include "LHJ/CRelicCollectionWidgetActor.h"
#include "LHM/Excavation/RelicsGround.h"
#include "LHM/Excavation/ExcavationWidgetActor.h"
#include "LHM/Excavation/BrushTool.h"
#include "LHM/Excavation/TweezersTool.h"

AMH_VRPlayer::AMH_VRPlayer()
{
	PrimaryActorTick.bCanEverTick = true;
	
	CHelpers::CreateComponent<USceneComponent>(this, &VRCompRoot, "CameraRoot", RootComponent);
	VRCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("VRCamera"));
	VRCamera->SetupAttachment(VRCompRoot);

	TeleportCircleA = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TeleportCircle"));
	TeleportCircleA->SetupAttachment(RootComponent);

	TeleportUIComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TeleportUIComponent"));
	TeleportUIComponent->SetupAttachment(RootComponent);

	CHelpers::CreateComponent<UMotionControllerComponent>(this, &RHandController, "RHandController", VRCompRoot);
	RHandController->SetTrackingMotionSource(FName("Right"));
	CHelpers::CreateComponent<UMotionControllerComponent>(this, &LHandController, "LHandController", VRCompRoot);
	LHandController->SetTrackingMotionSource(FName("Left"));
	CHelpers::CreateComponent<UMotionControllerComponent>(this, &RAimMotionController, "RAimMotionController", VRCompRoot);
	RAimMotionController->SetTrackingMotionSource(FName("RightAim"));
	CHelpers::CreateComponent<UMotionControllerComponent>(this, &LAimMotionController, "LAimMotionController", VRCompRoot);
	LAimMotionController->SetTrackingMotionSource(FName("LeftAim"));
	CHelpers::CreateComponent<UWidgetInteractionComponent>(this, &RWidgetInteractionComponent, "RWidgetInteractionComponent", RAimMotionController);

	CHelpers::CreateComponent<UNiagaraComponent>(this, &LineTraceEffectComponent, "LineTraceEffectComponent", RAimMotionController);
	LineTraceEffectComponent->SetVisibility(false);

	// 손 메쉬 설정
	HandLeft = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HandLeft"));
	HandRight = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HandRight"));
	HandLeft->SetupAttachment(LHandController);
	HandRight->SetupAttachment(RHandController);

	//그랩 컴프
	GrabComponent = CreateDefaultSubobject<UMH_GrabComp>(TEXT("GrabComponent"));

	//텔레포트 컴프
	TeleportComponent = CreateDefaultSubobject<UMH_TeleportComp>(TEXT("TeleportComponent"));

	{
		// WidgetInteraction 설정 - UI 상호작용을 위해 활성화
		RWidgetInteractionComponent->InteractionDistance = WidgetInteractionDistance; // UI 상호작용 거리
		RWidgetInteractionComponent->bEnableHitTesting = false; // UI 상호작용을 위해 활성화
		RWidgetInteractionComponent->bShowDebug = false;
		RWidgetInteractionComponent->InteractionSource = EWidgetInteractionSource::World;
		RWidgetInteractionComponent->TraceChannel = ECC_GameTraceChannel8;
		RWidgetInteractionComponent->PointerIndex = 0;
		RWidgetInteractionComponent->VirtualUserIndex = 0;
	}

	CHelpers::CreateActorComponent<UCMuseumComponent>(this, &MuseumComponent, "MuseumComponent");
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

	GrabComponent->SetHandComponent(RHandController);
	TeleportComponent->SetHandComponent(RHandController);
	TeleportComponent->SetTeleportVisual(TeleportCircleA, TeleportUIComponent);

	TeleportUIComponent->SetVisibility(false);
	TeleportCircleA->SetVisibility(false);

	// 월드에 존재하는 ACWorldMap을 찾아 저장
	for (TActorIterator<ACWorldMap> It(GetWorld()); It; ++It)
	{
		CachedWorldMap = *It;
		break;
	}

#pragma region Excavation
	CurrentLevel = GetWorld()->GetMapName();
	CurrentLevel.RemoveFromStart(GetWorld()->StreamingLevelsPrefix); // 레벨 이름 앞에 접두사 _ 제거

	UE_LOG(LogTemp, Log, TEXT("Current Level: %s"), *CurrentLevel);
	if (CurrentLevel == "LV_TestExcavation" || CurrentLevel == "LV_TestLobby")
	{
		// 월드에 있는 모든 ARelicsGround를 찾아 저장
		for (TActorIterator<ARelicsGround> It(GetWorld()); It; ++It)
		{
			RelicsGroundRefs.Add(*It);
		}

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;

		// Excavation Phase UI 
		if (ExcavationUIActorClass)
		{
			FVector SpawnLocation = FVector::ZeroVector;
			FRotator SpawnRotation = FRotator::ZeroRotator;

			ExcavationUIActor = GetWorld()->SpawnActor<AExcavationWidgetActor>(ExcavationUIActorClass, SpawnLocation, SpawnRotation, SpawnParams);
			if (ExcavationUIActor)
			{
				ExcavationUIActor->AttachToComponent(VRCamera, FAttachmentTransformRules::SnapToTargetNotIncludingScale, NAME_None);
				ExcavationUIActor->SetActorEnableCollision(false);
			}
		}
	}
#pragma endregion 발굴 레벨에서만 초기화
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
	EnhancedInput->BindAction(IA_MHTestTeleportStart, ETriggerEvent::Started, this, &AMH_VRPlayer::F_TeleportStart);
	EnhancedInput->BindAction(IA_MHTestTeleportEnd, ETriggerEvent::Completed, this, &AMH_VRPlayer::F_TeleportEnd);
	EnhancedInput->BindAction(IA_MHVRTeleport, ETriggerEvent::Started, this, &AMH_VRPlayer::F_TeleportStart);
	EnhancedInput->BindAction(IA_MHVRTeleport, ETriggerEvent::Completed, this, &AMH_VRPlayer::F_TeleportEnd);

	//Grab
	EnhancedInput->BindAction(IA_MHGrab, ETriggerEvent::Started, this, &AMH_VRPlayer::TryGrab);
	EnhancedInput->BindAction(IA_MHGrab, ETriggerEvent::Completed, this, &AMH_VRPlayer::TryUnGrab);
	EnhancedInput->BindAction(IA_AdjustTeleportDirection, ETriggerEvent::Triggered, this, &AMH_VRPlayer::HandleThumbstickInput);
	EnhancedInput->BindAction(IA_RotateHeldObject, ETriggerEvent::Triggered, this, &AMH_VRPlayer::HandleThumbstickInput);

	// Excavation Tool Actions
	EnhancedInput->BindAction(IA_ExcavationTool1, ETriggerEvent::Started, this, &AMH_VRPlayer::ExcavationTool1);
	EnhancedInput->BindAction(IA_ExcavationTool2, ETriggerEvent::Started, this, &AMH_VRPlayer::ExcavationTool2);
	EnhancedInput->BindAction(IA_ExcavationTool3, ETriggerEvent::Started, this, &AMH_VRPlayer::ExcavationTool3);
	EnhancedInput->BindAction(IA_ExcavationTool4, ETriggerEvent::Started, this, &AMH_VRPlayer::ExcavationTool4);
	EnhancedInput->BindAction(IA_ExcavationDetect, ETriggerEvent::Triggered, this, &AMH_VRPlayer::ExcavationDetectStart);
	EnhancedInput->BindAction(IA_ExcavationDetect, ETriggerEvent::Completed, this, &AMH_VRPlayer::ExcavationDetectEnd);
	EnhancedInput->BindAction(IA_ExcavationDig, ETriggerEvent::Started, this, &AMH_VRPlayer::ExcavationDigStart);
	EnhancedInput->BindAction(IA_ExcavationDig, ETriggerEvent::Completed, this, &AMH_VRPlayer::ExcavationDigEnd);
	EnhancedInput->BindAction(IA_ExcavationBrush, ETriggerEvent::Started, this, &AMH_VRPlayer::ExcavationBrushStart);
	EnhancedInput->BindAction(IA_ExcavationBrush, ETriggerEvent::Completed, this, &AMH_VRPlayer::ExcavationBrushEnd);
	EnhancedInput->BindAction(IA_ExcavationCollect, ETriggerEvent::Triggered, this, &AMH_VRPlayer::ExcavationCollectStart);
	EnhancedInput->BindAction(IA_ExcavationCollect, ETriggerEvent::Completed, this, &AMH_VRPlayer::ExcavationCollectEnd);

	if (MuseumComponent) MuseumComponent->SetupPlayerInputComponent(EnhancedInput);
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
		
		PreTeleportState = GetPlayerState();

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
			FVector AddHeight = FVector(
				0, 0, GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + AdditiveTeleportHeight);
			SetActorLocation(OutLocation + AddHeight);
			TeleportDistanceFactor = 1.0f;
		}

		SetPlayerState(EPlayerVRState::Idle);

		// 이전 상태로 복구 (도구를 들고 있었으면 다시 UsingTool로)
		if (PreTeleportState == EPlayerVRState::UsingTool)
		{
			SetPlayerState(PreTeleportState);
		}
		else if (PreTeleportState == EPlayerVRState::Excavating)
		{
			SetPlayerState(PreTeleportState);
		}
		else
		{
			SetPlayerState(EPlayerVRState::Idle);
		}
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
	// WidgetInteraction 활성화
	SetWidgetInteractionUsing(true);
	// LineTraceEffect 활성화/비활성화
	SetUseLineTraceEffect(true);
	
	// 상호작용이 가능한 UI에 닿아있는지 확인
	FVector Start = RWidgetInteractionComponent->GetComponentLocation();
	FVector End = FVector::ZeroVector;
	bInteracteAnyComponent = false;
	if (IsPointingAtWidget())
	{
		SetClickAndWidgetActivation(true);
		// Hit된 위치로 LineTraceEffect를 그림
		bInteracteAnyComponent = true;
		End = RWidgetInteractionComponent->GetLastHitResult().ImpactPoint;
		UpdateDrawLineTraceEffect(Start, End);
	}
	else
	{
		SetClickAndWidgetActivation(false);		
	}

	// 박물관 레벨에서만 작동
	if (CurrentLevel.ToLower().Contains("museum"))
	{
		// 유물 설치
		if (MuseumComponent && MuseumComponent->GetMuseumState() == Decorate)
			MuseumComponent->PlaceRelic();
	}
	// 로비 레벨에서만 작동
	else if (CurrentLevel.ToLower().Contains("lobby"))
	{
		// 월드맵 상호작용
		// 함수 내부에서 UI 업데이트까지 수행
		TryWorldMapInteraction(IA_Instance);
	}
	
	// 아무 곳에도 닿지 않았으면
	if (!bInteracteAnyComponent)
	{
		// 사정거리 끝까지 그림
		End = RWidgetInteractionComponent->GetComponentLocation() + RWidgetInteractionComponent->GetForwardVector() * WidgetInteractionDistance;
		// DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 0.0f, 0.0f, 1.0f);
		UpdateDrawLineTraceEffect(Start, End);
	}
}

void AMH_VRPlayer::TriggerInteractCompleted()
{	
	// 저장된 Widget가 있든 없든
	// 좌클릭을 해제하고, Widget을 초기화한다.
	SetClickAndWidgetActivation(false);
	// LineTraceEffect 활성화/비활성화
	SetUseLineTraceEffect(false);
	
	// WidgetInteraction 비활성화
	if (!CurrentLevel.ToLower().Contains("museum"))
		SetWidgetInteractionUsing(false);
	else
	{
		if (!(MuseumComponent && MuseumComponent->GetMuseumState() == Decorate))
			SetWidgetInteractionUsing(false);
	}
		
	if (CurrentLevel.ToLower().Contains("lobby"))
	{
		// WorldMap Interaction 초기화
		ResetWorldMapInteraction();			
	}
}

void AMH_VRPlayer::ExcavationTool1()
{
	if (!DetectionTool)
	{
		if (ShovelTool)
		{
			ShovelTool->Destroy();
			ShovelTool = nullptr;
		}
		if (BrushTool)
		{
			BrushTool->Destroy();
			BrushTool = nullptr;
		}
		if (TweezersTool)
		{
			TweezersTool->Destroy();
			TweezersTool = nullptr;
		}
		
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;

		if(DetectionToolClass)
		{
			DetectionTool = GetWorld()->SpawnActor<ADetectorTool>(DetectionToolClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
			if (DetectionTool)
			{
				DetectionTool->AttachToComponent(HandRight, FAttachmentTransformRules::SnapToTargetNotIncludingScale, "Socket_Tool1");

				SetPlayerState(EPlayerVRState::UsingTool);
			}
		}
	}
	else
	{
		DetectionTool->Destroy();
		DetectionTool = nullptr;
		SetPlayerState(EPlayerVRState::Idle);
	}
}

void AMH_VRPlayer::ExcavationTool2()
{
	if (!ShovelTool)
	{
		if (DetectionTool)
		{
			DetectionTool->Destroy();
			DetectionTool = nullptr;
		}
		if (BrushTool)
		{
			BrushTool->Destroy();
			BrushTool = nullptr;
		}
		if (TweezersTool)
		{
			TweezersTool->Destroy();
			TweezersTool = nullptr;
		}

		if(ShovelToolClass)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;

			ShovelTool = GetWorld()->SpawnActor<AShovelTool>(ShovelToolClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
			if (ShovelTool)
			{
				ShovelTool->AttachToComponent(HandRight, FAttachmentTransformRules::SnapToTargetNotIncludingScale, "Socket_Tool2");

				// RelicsGround의 Shovel 할당
				for (auto RelicsGroundRef : RelicsGroundRefs)
				{
					if (RelicsGroundRef) RelicsGroundRef->SetShovelReference(ShovelTool);
				}

				SetPlayerState(EPlayerVRState::UsingTool);
			}
		}
	}
	else
	{
		ShovelTool->Destroy();
		ShovelTool = nullptr;
		SetPlayerState(EPlayerVRState::Idle);
	}
}

void AMH_VRPlayer::ExcavationTool3()
{
	if (!BrushTool)
	{
		if (DetectionTool)
		{
			DetectionTool->Destroy();
			DetectionTool = nullptr;
		}
		if (ShovelTool)
		{
			ShovelTool->Destroy();
			ShovelTool = nullptr;
		}
		if (TweezersTool)
		{
			TweezersTool->Destroy();
			TweezersTool = nullptr;
		}

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;

		if(BrushToolClass)
		{
			BrushTool = GetWorld()->SpawnActor<ABrushTool>(BrushToolClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
			if (BrushTool)
			{
				BrushTool->AttachToComponent(HandRight, FAttachmentTransformRules::SnapToTargetNotIncludingScale, "Socket_Tool3");

				SetPlayerState(EPlayerVRState::UsingTool);
			}
		}
	}
	else
	{
		BrushTool->Destroy();
		BrushTool = nullptr;
		SetPlayerState(EPlayerVRState::Idle);
	}
}

void AMH_VRPlayer::ExcavationTool4()
{
	if (!TweezersTool)
	{
		if (DetectionTool)
		{
			DetectionTool->Destroy();
			DetectionTool = nullptr;
		}
		if (ShovelTool)
		{
			ShovelTool->Destroy();
			ShovelTool = nullptr;
		}
		if (BrushTool)
		{
			BrushTool->Destroy();
			BrushTool = nullptr;
		}

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;

		if(TweezersToolClass)
		{
			TweezersTool = GetWorld()->SpawnActor<ATweezersTool>(TweezersToolClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
			if (TweezersTool)
			{
				TweezersTool->AttachToComponent(HandRight, FAttachmentTransformRules::SnapToTargetNotIncludingScale, "Socket_Tool4");
				
				TweezersTool->SetAttachBase(RHandController);

				SetPlayerState(EPlayerVRState::UsingTool);
			}
		}
	}
	else
	{
		TweezersTool->Destroy();
		TweezersTool = nullptr;
		SetPlayerState(EPlayerVRState::Idle);
	}
}

void AMH_VRPlayer::ExcavationDetectStart()
{
	if (!DetectionTool) return;
	if (bIsUIInteractionActive) return;
	if (GetPlayerState() != EPlayerVRState::UsingTool) return;

	DetectionTool->SetIsDetecting(true);
	SetPlayerState(EPlayerVRState::Excavating);
}

void AMH_VRPlayer::ExcavationDetectEnd()
{
	if (!DetectionTool) return;
	if (GetPlayerState() != EPlayerVRState::Excavating) return;

	DetectionTool->SetIsDetecting(false);
	SetPlayerState(EPlayerVRState::UsingTool);
}

void AMH_VRPlayer::ExcavationDigStart()
{
	if (!ShovelTool) return;
	if (bIsUIInteractionActive) return;
	if (GetPlayerState() != EPlayerVRState::UsingTool) return;

	ShovelTool->SetIsDigging(true);
	SetPlayerState(EPlayerVRState::Excavating);
}

void AMH_VRPlayer::ExcavationDigEnd()
{
	if (!ShovelTool) return;
	if (GetPlayerState() != EPlayerVRState::Excavating) return;

	ShovelTool->SetIsDigging(false);
	SetPlayerState(EPlayerVRState::UsingTool);
}

void AMH_VRPlayer::ExcavationBrushStart()
{
	if (!BrushTool) return;
	if (bIsUIInteractionActive) return;
	if (GetPlayerState() != EPlayerVRState::UsingTool) return;

	BrushTool->SetIsBrushing(true);
	SetPlayerState(EPlayerVRState::Excavating);
}

void AMH_VRPlayer::ExcavationBrushEnd()
{
	if (!BrushTool) return;
	if (GetPlayerState() != EPlayerVRState::Excavating) return;

	BrushTool->SetIsBrushing(false);
	SetPlayerState(EPlayerVRState::UsingTool);
}

void AMH_VRPlayer::ExcavationCollectStart()
{
	if (!TweezersTool) return;
	if (bIsUIInteractionActive) return;
	if (GetPlayerState() != EPlayerVRState::UsingTool) return;

	TweezersTool->SetIsPickingUp(true);
	SetPlayerState(EPlayerVRState::Excavating);
}

void AMH_VRPlayer::ExcavationCollectEnd()
{
	if (!TweezersTool) return;
	if (GetPlayerState() != EPlayerVRState::Excavating) return;

	TweezersTool->SetIsPickingUp(false);
	SetPlayerState(EPlayerVRState::UsingTool);
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
		Start = RHandController->GetComponentLocation();
		Velocity = RHandController->GetForwardVector() * 1000.f * TeleportDistanceFactor; // 강도는 상황에 맞게 조절
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
		if (DetectionTool) Params.AddIgnoredActor(DetectionTool);
		if (ShovelTool) Params.AddIgnoredActor(ShovelTool);
		if (BrushTool) Params.AddIgnoredActor(BrushTool);
		if (TweezersTool) Params.AddIgnoredActor(TweezersTool);

		if (GetWorld()->LineTraceSingleByChannel(Hit, LastPos, Pos, ECC_Visibility, Params))
		{
			AActor* HitActor = Hit.GetActor();
			if (!HitActor) continue;

			// Grab 처리
			if (!FocusedGrabbableActor && HitActor->ActorHasTag("Grabable"))
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
	if (GetPlayerState() == EPlayerVRState::UsingTool || GetPlayerState() == EPlayerVRState::Excavating) return;
	if (!FocusedGrabbableActor) return;

	UPrimitiveComponent* HitComp = Cast<UPrimitiveComponent>(
		FocusedGrabbableActor->GetComponentByClass(UPrimitiveComponent::StaticClass()));

	if (FocusedGrabbableActor->IsA(ACRelicBase::StaticClass())) // 그랩 대상이 유물이면, 피직스를 켜준다
	{
		GrabRelicActor = FocusedGrabbableActor;
		HitComp->SetSimulatePhysics(true);
	}
	
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
	if (GetPlayerState() == EPlayerVRState::UsingTool || GetPlayerState() == EPlayerVRState::Excavating) return;
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

void AMH_VRPlayer::ToggleMenu_Implementation()
{
}

void AMH_VRPlayer::TryWorldMapInteraction(const FInputActionInstance& IA_Instance)
{
	if (!RWidgetInteractionComponent) return;

	FVector Start = RWidgetInteractionComponent->GetComponentLocation();
	FVector End = Start + (RWidgetInteractionComponent->GetForwardVector() * WidgetInteractionDistance);

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_GameTraceChannel3, Params))
	{
		if (!HitResult.GetActor()->IsA(ACWorldMap::StaticClass())) return;
		UStaticMeshComponent* HitMesh = Cast<UStaticMeshComponent>(HitResult.GetComponent());
		if (HitMesh) Cast<ACWorldMap>(HitResult.GetActor())->EnableCompOutline(HitMesh);

		End = HitResult.ImpactPoint;
		UpdateDrawLineTraceEffect(Start, End);
		bInteracteAnyComponent = true;
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
  
// UI 상호작용 핵심 함수들
void AMH_VRPlayer::HandleUIInteraction(const FInputActionInstance& IA_Instance)
{
	UMotionControllerComponent* MotionController = RHandController;
	UWidgetInteractionComponent* WidgetInteraction = RWidgetInteractionComponent;

	if (!MotionController || !WidgetInteraction) return;

	WidgetInteraction->bShowDebug = true;
	WidgetInteraction->bEnableHitTesting = true;
    UWidgetComponent* HitWidgetComponent = nullptr;
    if (IsPointingAtUI(MotionController, HitWidgetComponent))
    {
        if (!bIsUIInteractionActive)
        {
            EnableWidgetInteraction(MotionController);
            CurrentFocusedUI = HitWidgetComponent;
            bIsUIInteractionActive = true;
        }
		WidgetInteraction->PressPointerKey(EKeys::LeftMouseButton);
    }
    else
    {
        if (bIsUIInteractionActive)
        {
            if (MuseumComponent && MuseumComponent->GetMuseumState() == EMuseumState::Decorate) return;
            DisableWidgetInteraction();
        }
    }
}

bool AMH_VRPlayer::IsPointingAtUI(UMotionControllerComponent* MotionController, UWidgetComponent*& OutWidgetComponent)
{
	if (!MotionController || !RWidgetInteractionComponent) 
	{
		OutWidgetComponent = nullptr;
		return false;
	}

	// WidgetInteraction 컴포넌트가 현재 UI를 가리키고 있는지 확인
	UWidgetComponent* HoveredWidget = RWidgetInteractionComponent->GetHoveredWidgetComponent();
	if (HoveredWidget && HoveredWidget->IsVisible())
	{
		OutWidgetComponent = HoveredWidget;
		return true;
	}

	// WidgetInteraction의 Hit Result를 직접 확인
	FHitResult HitResult = RWidgetInteractionComponent->GetLastHitResult();
	if (HitResult.GetComponent() && HitResult.GetComponent()->IsA(UWidgetComponent::StaticClass()))
	{
		OutWidgetComponent = Cast<UWidgetComponent>(HitResult.GetComponent());
		return true;
	}

	OutWidgetComponent = nullptr;
	return false;
}

void AMH_VRPlayer::EnableWidgetInteraction(UMotionControllerComponent* MotionController)
{
	if (!MotionController) return;

	// WidgetInteraction 컴포넌트 활성화
	ActiveWidgetInteraction = RWidgetInteractionComponent;
	ActiveWidgetInteraction->SetActive(true);
	ActiveWidgetInteraction->bEnableHitTesting = true;

	UE_LOG(LogTemp, Log, TEXT("[VR] WidgetInteraction 활성화"));
}

void AMH_VRPlayer::DisableWidgetInteraction()
{
	if (ActiveWidgetInteraction)
	{
		ActiveWidgetInteraction->SetActive(false);
		ActiveWidgetInteraction->bEnableHitTesting = false;
		ActiveWidgetInteraction = nullptr;
	}

	CurrentFocusedUI = nullptr;
	bIsUIInteractionActive = false;

	UE_LOG(LogTemp, Log, TEXT("[VR] WidgetInteraction 비활성화"));
}

void AMH_VRPlayer::SetWidgetInteractionUsing(bool bUsing)
{
	RWidgetInteractionComponent->SetActive(bUsing);
	RWidgetInteractionComponent->bEnableHitTesting = bUsing;
	// RWidgetInteractionComponent->bShowDebug = bUsing;
	if (!bUsing)
	{
		if(RWidgetInteractionComponent->IsOverFocusableWidget())
		{
			RWidgetInteractionComponent->ReleaseKey(EKeys::LeftMouseButton);
		}
	}
}

bool AMH_VRPlayer::IsPointingAtWidget()
{
	if (!RWidgetInteractionComponent) return false;
	return RWidgetInteractionComponent->IsOverInteractableWidget();
}

void AMH_VRPlayer::SetClickAndWidgetActivation(bool bUsing)
{
	// 좌클릭 입력/해제
	SetWidgetInteractionClick(bUsing);
	// Widget 등록/초기화
	SetWidgetComponent(bUsing);
}

void AMH_VRPlayer::SetWidgetInteractionClick(bool bPress)
{
	if (!RWidgetInteractionComponent) return;
	if (bPress)
		RWidgetInteractionComponent->PressPointerKey(EKeys::LeftMouseButton);
	else
		RWidgetInteractionComponent->ReleasePointerKey(EKeys::LeftMouseButton);
}

void AMH_VRPlayer::SetWidgetComponent(bool bSet)
{
	if (bSet)
	{
		FHitResult HitResult = RWidgetInteractionComponent->GetLastHitResult();
		if (HitResult.GetComponent() && HitResult.GetComponent()->IsA(UWidgetComponent::StaticClass()))
		{
			CurrentFocusedUI = Cast<UWidgetComponent>(HitResult.GetComponent());
		}
	}
	else
	{
		CurrentFocusedUI = nullptr;
	}
}

void AMH_VRPlayer::SetUseLineTraceEffect(bool bUse)
{
	if (!LineTraceEffectComponent) return;
	LineTraceEffectComponent->SetVisibility(bUse);
	UpdateDrawLineTraceEffect(FVector::ZeroVector, FVector::ZeroVector);
}

void AMH_VRPlayer::UpdateDrawLineTraceEffect(const FVector& Start, const FVector& End)
{
	if (!LineTraceEffectComponent) return;
	TArray<FVector> LineTracePoints;
	LineTracePoints.Emplace(Start);
	LineTracePoints.Emplace(End);
	UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector(
			LineTraceEffectComponent, TEXT("User.PointArray"), LineTracePoints);
}
