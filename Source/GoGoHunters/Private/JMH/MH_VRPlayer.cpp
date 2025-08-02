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
	CHelpers::CreateComponent<UMotionControllerComponent>(this, &RAimMotionController, "RAimMotionController", RootComponent);
	RAimMotionController->SetTrackingMotionSource(FName("RightAim"));
	CHelpers::CreateComponent<UMotionControllerComponent>(this, &LAimMotionController, "LAimMotionController", RootComponent);
	LAimMotionController->SetTrackingMotionSource(FName("LeftAim"));
	CHelpers::CreateComponent<UWidgetInteractionComponent>(this, &RWidgetInteractionComponent, "RWidgetInteractionComponent", RAimMotionController);
	CHelpers::CreateComponent<UWidgetInteractionComponent>(this, &LWidgetInteractionComponent, "LWidgetInteractionComponent", LAimMotionController);

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
		RWidgetInteractionComponent->InteractionDistance = 800.f; // UI 상호작용 거리 증가
		RWidgetInteractionComponent->bEnableHitTesting = false; // UI 상호작용을 위해 활성화
		RWidgetInteractionComponent->bShowDebug = false;
		RWidgetInteractionComponent->InteractionSource = EWidgetInteractionSource::World;
		RWidgetInteractionComponent->TraceChannel = ECC_GameTraceChannel8;
		RWidgetInteractionComponent->PointerIndex = 0;
		RWidgetInteractionComponent->VirtualUserIndex = 0;
		
		LWidgetInteractionComponent->InteractionDistance = 800.f; // UI 상호작용 거리 증가
		LWidgetInteractionComponent->bEnableHitTesting = false; // UI 상호작용을 위해 활성화
		LWidgetInteractionComponent->bShowDebug = false;
		LWidgetInteractionComponent->InteractionSource = EWidgetInteractionSource::World;
		LWidgetInteractionComponent->TraceChannel = ECC_GameTraceChannel8;
		LWidgetInteractionComponent->PointerIndex = 1;
		LWidgetInteractionComponent->VirtualUserIndex = 1;
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
	FString CurrentLevel = GetWorld()->GetMapName();
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
		if (ExcavationUIActorClass = LoadClass<AExcavationWidgetActor>(nullptr, TEXT("/Game/LHM/BP/Excavation/BP_ExcavationWidgetActor.BP_ExcavationWidgetActor_C")))
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
	
	// UI 상호작용 상태 업데이트 (트리거를 누르지 않았을 때도 UI 감지)
	// if (CurrentState == EPlayerVRState::Idle && !bIsUIInteractionActive)
	// {
	// 	// 양손에서 UI 감지 (시각적 피드백용)
	// 	UWidgetComponent* TempWidget = nullptr;
	// 	if (IsPointingAtUI(RHandController, TempWidget) || IsPointingAtUI(LHandController, TempWidget))
	// 	{
	// 		// UI를 가리키고 있지만 아직 상호작용하지 않은 상태
	// 		// 여기서 UI 하이라이트 효과 등을 추가할 수 있음
	// 	}
	 // }
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
			SetActorLocation(OutLocation + FVector(0.f, 0.f, 100.f));
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
    // UI 상호작용
    HandleUIInteraction(IA_Instance);

    // 도구 상태 체크
    if(GetPlayerState() == EPlayerVRState::UsingTool 
	|| GetPlayerState() == EPlayerVRState::Excavating) return;

    // 월드맵 상호작용
    TryWorldMapInteraction(IA_Instance);

    // 유물 설치
    if (MuseumComponent)
    {
        if (MuseumComponent->GetMuseumState() == Decorate)
            MuseumComponent->PlaceRelic();
    }
}

void AMH_VRPlayer::TriggerInteractCompleted()
{
	// UI 상호작용 종료 처리
	if (bIsUIInteractionActive)
	{
		// 현재 활성화된 WidgetInteraction으로 마우스 버튼 해제
		if (ActiveWidgetInteraction)
		{
			ActiveWidgetInteraction->ReleasePointerKey(EKeys::LeftMouseButton);
			ActiveWidgetInteraction->bEnableHitTesting = false;
			ActiveWidgetInteraction->bShowDebug = false;
		}

		if (!(MuseumComponent && MuseumComponent->GetMuseumState() == EMuseumState::Decorate))
			DisableWidgetInteraction();
	}

	// 월드맵 상호작용 리셋
	ResetWorldMapInteraction();
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

		if (DetectionToolClass = LoadClass<ADetectorTool>(nullptr, TEXT("/Game/LHM/BP/Excavation/BP_DetectorTool.BP_DetectorTool_C")))
		{
			DetectionTool = GetWorld()->SpawnActor<ADetectorTool>(DetectionToolClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
			if (DetectionTool)
			{
				//USceneComponent* HandSocket = RHandController;
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

		if (ShovelToolClass = LoadClass<AShovelTool>(nullptr, TEXT("/Game/LHM/BP/Excavation/BP_ShovelTool.BP_ShovelTool_C")))
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;

			FVector SpawnLocation = FVector(-7, 1.5, -7); // (X=-7.000000,Y=1.500000,Z=-7.000000)
			FRotator SpawnRotation = FRotator(0, 55, 90); // (Pitch=0.000000,Yaw=55.000000,Roll=90.000000)

			ShovelTool = GetWorld()->SpawnActor<AShovelTool>(ShovelToolClass, SpawnLocation, SpawnRotation, SpawnParams);
			if (ShovelTool)
			{
				//USceneComponent* HandSocket = RHandController;
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

		if (BrushToolClass = LoadClass<ABrushTool>(nullptr, TEXT("/Game/LHM/BP/Excavation/BP_BrushTool.BP_BrushTool_C")))
		{
			BrushTool = GetWorld()->SpawnActor<ABrushTool>(BrushToolClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
			if (BrushTool)
			{
				//USceneComponent* HandSocket = RHandController;
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

		if (TweezersToolClass = LoadClass<ATweezersTool>(nullptr, TEXT("/Game/LHM/BP/Excavation/BP_TweezersTool.BP_TweezersTool_C")))
		{
			TweezersTool = GetWorld()->SpawnActor<ATweezersTool>(TweezersToolClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
			if (TweezersTool)
			{
				//USceneComponent* HandSocket = RHandController;
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
	UMotionControllerComponent* MotionController = nullptr;
	if (IA_Instance.GetSourceAction() == IA_MHInteract) MotionController = RHandController;
	else if (IA_Instance.GetSourceAction() == IA_MHInteract_L) MotionController = LHandController;

	if (!MotionController) return;

	FVector Start = MotionController->GetComponentLocation();
	FVector End = Start + (MotionController->GetForwardVector() * 800.f);

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
  
// UI 상호작용 핵심 함수들
void AMH_VRPlayer::HandleUIInteraction(const FInputActionInstance& IA_Instance)
{
	UMotionControllerComponent* MotionController = nullptr;
	UWidgetInteractionComponent* WidgetInteraction = nullptr;

	// 입력 액션 종류와 무관하게 손/위젯 매칭
	if (IA_Instance.GetSourceAction() == IA_MHInteract)
	{
		MotionController = RHandController;
		WidgetInteraction = RWidgetInteractionComponent;
	}
	else if (IA_Instance.GetSourceAction() == IA_MHInteract_L)
	{
		MotionController = LHandController;
		WidgetInteraction = LWidgetInteractionComponent;
	}
	else if (IA_Instance.GetSourceAction() == IA_ExcavationDetect
			 || IA_Instance.GetSourceAction() == IA_ExcavationDig
			 || IA_Instance.GetSourceAction() == IA_ExcavationBrush
			 || IA_Instance.GetSourceAction() == IA_ExcavationCollect)
	{
		MotionController = RHandController;
		WidgetInteraction = RWidgetInteractionComponent;
	}
	
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
	if (!MotionController) return false;

	FVector Start = MotionController->GetComponentLocation();
	FVector End = Start + (MotionController->GetForwardVector() * 800.f);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	if (DetectionTool) QueryParams.AddIgnoredActor(DetectionTool);
	if (ShovelTool) QueryParams.AddIgnoredActor(ShovelTool);
	if (BrushTool) QueryParams.AddIgnoredActor(BrushTool);
	if (TweezersTool) QueryParams.AddIgnoredActor(TweezersTool);

	// UI 전용 레이캐스트 (WidgetComponent 찾기)
	TArray<FHitResult> HitResults;
	if (GetWorld()->LineTraceMultiByChannel(HitResults, Start, End, ECC_GameTraceChannel8, QueryParams))
	{
		for (const FHitResult& Hit : HitResults)
		{
			// WidgetComponent 확인
			UWidgetComponent* WidgetComponent = Cast<UWidgetComponent>(Hit.GetComponent());
			if (WidgetComponent && WidgetComponent->IsVisible())
			{
				OutWidgetComponent = WidgetComponent;
				return true;
			}
		}
	}

	OutWidgetComponent = nullptr;
	return false;
}

void AMH_VRPlayer::EnableWidgetInteraction(UMotionControllerComponent* MotionController)
{
	if (!MotionController) return;

	// 해당 손의 WidgetInteraction 활성화
	if (MotionController == RHandController)
	{
		ActiveWidgetInteraction = RWidgetInteractionComponent;
	}
	else if (MotionController == LHandController)
	{
		ActiveWidgetInteraction = LWidgetInteractionComponent;
	}

	if (ActiveWidgetInteraction)
	{
		ActiveWidgetInteraction->SetActive(true);
		ActiveWidgetInteraction->bEnableHitTesting = true;

		UE_LOG(LogTemp, Log, TEXT("[VR] WidgetInteraction 활성화 - %s"),
			   MotionController == RHandController ? TEXT("오른손") : TEXT("왼손"));
	}
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
