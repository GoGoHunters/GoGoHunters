// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputAction.h"
#include "MH_ZoneBase.h"
#include "MH_VRPlayer.generated.h"

class ACTutorialManager;
class ACRelicCollectionWidgetActor;
class UCMuseumComponent;
class ACWorldMap;
class UMotionControllerComponent;
class UWidgetInteractionComponent;
class UWidgetComponent;
class USpringArmComponent;

/*
 * 텔레포트 조건 = Teleportable 액터 태그
 * 그랩 조건 = Grabbable 액터 태그
 */


UENUM(BlueprintType)
enum class EPlayerVRState : uint8
{
	Idle			UMETA(DisplayName = "Idle"),			// 아무것도 하지 않는 기본 상태
	UsingTool		UMETA(DisplayName = "Using Tool"),		// 도구를 손에 든 상태 (아직 사용은 안함)
	Excavating		UMETA(DisplayName = "Excavating"),		// 도구를 실제로 사용하여 발굴 중
	GrabbingObject	UMETA(DisplayName = "Grabbing Object"),	// 유물(또는 도구 등)을 손에 잡은 상태
	Inspecting		UMETA(DisplayName = "Inspecting"),		// 손에 든 유물/오브젝트를 관찰(회전/확대) 중
	PlacingObject	UMETA(DisplayName = "Placing Object"),	// 나만의 박물관에서 오브젝트를 배치 중
	Teleporting		UMETA(DisplayName = "Teleporting"),		// 틸레포트 이동중
	Disabled		UMETA(DisplayName = "Disabled")			// 입력 및 조작이 비활성화된 상태 (UI 열림 등)
};

UCLASS()
class GOGOHUNTERS_API AMH_VRPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMH_VRPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	USceneComponent* VRCompRoot;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UCameraComponent* VRCamera;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class USkeletalMeshComponent* HandLeft;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class USkeletalMeshComponent* HandRight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UMotionControllerComponent* RHandController;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UMotionControllerComponent* LHandController;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UMotionControllerComponent* LAimMotionController;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UMotionControllerComponent* RAimMotionController;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UWidgetInteractionComponent* RWidgetInteractionComponent;

	// 현재 플레이어의 상태
	UPROPERTY(BlueprintReadWrite, Category = "State")
	EPlayerVRState CurrentState = EPlayerVRState::Idle;
	
	UFUNCTION(BlueprintCallable, Category = "State")
	void SetPlayerState(EPlayerVRState NewState);

	UFUNCTION(BlueprintPure, Category = "State")
	EPlayerVRState GetPlayerState() const;

	// 발굴 중 상태 복구용
	EPlayerVRState PreTeleportState = EPlayerVRState::Idle;

	UPROPERTY()
	AActor* FocusedGrabbableActor;
	UPROPERTY()
	TObjectPtr<AActor> GrabRelicActor;
public:
	
	//IA////////////////////////////////////////////////////////
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	class UInputMappingContext* InputMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* IA_MHGrab;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* IA_MHGrab_L;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* IA_MHInteract;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* IA_MHInteract_L;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* IA_MHTestTeleportStart;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* IA_MHTestTeleportEnd;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* IA_MHVRTeleport;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* IA_MHTurn;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* IA_MHVRTurn;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* IA_MHLookUp;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* IA_AdjustTeleportDirection;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* IA_RotateHeldObject;

#pragma region Excavation
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* IA_ExcavationTool1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* IA_ExcavationTool2;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* IA_ExcavationTool3;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* IA_ExcavationTool4;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* IA_ExcavationDetect;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* IA_ExcavationDig;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* IA_ExcavationBrush;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* IA_ExcavationCollect;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* IA_MHStand;

	void ExcavationTool1();
	void ExcavationTool2();
	void ExcavationTool3();
	void ExcavationTool4();
	void ExcavationDetectStart();
	void ExcavationDetectEnd();
	void ExcavationDigStart();
	void ExcavationDigEnd();
	void ExcavationBrushStart();
	void ExcavationBrushEnd();
	void ExcavationCollectStart();
	void ExcavationCollectEnd();
	void SitOrStand();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Excavation")
	TSubclassOf<class ADetectorTool> DetectionToolClass;
	UPROPERTY(BlueprintReadOnly)
	class ADetectorTool* DetectionTool;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Excavation")
	TSubclassOf<class AShovelTool> ShovelToolClass;
	UPROPERTY(BlueprintReadOnly)
	class AShovelTool* ShovelTool;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Excavation")
	TSubclassOf<class ABrushTool> BrushToolClass;
	UPROPERTY(BlueprintReadOnly)
	class ABrushTool* BrushTool;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Excavation")
	TSubclassOf<class ATweezersTool> TweezersToolClass;
	UPROPERTY(BlueprintReadOnly)
	class ATweezersTool* TweezersTool;

	UPROPERTY(BlueprintReadOnly)
	TArray<class ARelicsGround*> RelicsGroundRefs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Excavation")
	TSubclassOf<class AExcavationWidgetActor> ExcavationUIActorClass;

	UPROPERTY()
	class AExcavationWidgetActor* ExcavationUIActor;

#pragma endregion 발굴 관련

	//마우스 회전방지
	bool bUseMouse = true;
	
	//VR 모드 사용
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	bool bUseVR = true;

	//Line Trace
	void UpdateInteractionLine();

	//Grab////////////////////////////////////////////////////////////////////

	UPROPERTY()
	class UMH_GrabComp* GrabComponent;
	
	// 잡기 함수들
	void TryGrab(const FInputActionInstance& IA_Instance);
	void TryUnGrab(const FInputActionInstance& IA_Instance);
	void DropForMuseumStateChange();
		
	// 썸스틱 입력 저장
	FVector2D HeldObjectStickInput = FVector2D::ZeroVector;

	//test Key
	void TestTurn(const FInputActionValue& Value);
	void TestLookUp(const FInputActionValue& Value);
	UFUNCTION()
	void TriggerInteract(const FInputActionInstance& IA_Instance);
	UFUNCTION()
	void TriggerInteractCompleted();
	
	UFUNCTION(exec)
	void ActiveDebugDraw();

	// 회전 입력 처리 함수
	void RotateHeldObject(const struct FInputActionValue& Value);

	//텔레포트//////////////////////////////////////////////////////////////

	UPROPERTY()
	class UMH_TeleportComp* TeleportComponent;

	TArray<FVector> Lines;
	
	//텔레포트 곡선 방식
	//텔레포트 구역 원
	UPROPERTY(VisibleAnywhere)
	class UNiagaraComponent* TeleportCircleA;

	//텔레포트 나이아가라
	UPROPERTY(VisibleAnywhere)
	class UNiagaraComponent* TeleportUIComponent;

	//텔레포트 진행여부
	bool bTeleporting = false;
	bool bIsDebugDraw = true;

	//키 이벤트 바인딩 함수
	void F_TeleportStart(const struct FInputActionValue& Value);
	void F_TeleportEnd(const struct FInputActionValue& Value);

	//텔레포트 or Grab
	void HandleThumbstickInput(const FInputActionValue& Value);

	//텔레포트 길이조절
	void AdjustTeleportDirection(const FInputActionValue& Value);

	float TeleportDistanceFactor = 1.0f;
	float TeleportAdjustSpeed = 1.0f;
	
	//VR SnapTurn 
	void VRTurn(const FInputActionValue& Value);

	UPROPERTY(EditAnywhere, Category="VR Movement")
	float SnapTurnAngle = 15.f;

	UFUNCTION(BlueprintNativeEvent)
	void ToggleMenu();

	// LineTrace & WidgetInteraction Line Update Function
	void UpdateDrawLineTraceEffect(const FVector& Start, const FVector& End);
	const float GetWidgetInteractionDistance() const { return WidgetInteractionDistance; }
	void SetUseLineTraceEffect(bool bUse);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void CameraFade();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetPlayerStandOrSit(bool bStand);
private:
	UPROPERTY()
	TObjectPtr<ACWorldMap> CachedWorldMap = nullptr;

	void TryWorldMapInteraction(const FInputActionInstance& IA_Instance);
	void ResetWorldMapInteraction();

	// UI 상호작용 관련 변수들
	UPROPERTY()
	TObjectPtr<UWidgetComponent> CurrentFocusedUI = nullptr;
	
	UPROPERTY()
	TObjectPtr<UWidgetInteractionComponent> ActiveWidgetInteraction = nullptr;
	
	bool bIsUIInteractionActive = false;
	
	// UI 상호작용 함수들
	void HandleUIInteraction(const FInputActionInstance& IA_Instance);
	
	// UI 감지 함수
	bool IsPointingAtUI(UMotionControllerComponent* MotionController, UWidgetComponent*& OutWidgetComponent);
	
	// WidgetInteraction 활성화/비활성화
	void EnableWidgetInteraction(UMotionControllerComponent* MotionController);
	void DisableWidgetInteraction();

	// Museum Component
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	UCMuseumComponent* MuseumComponent;

	UPROPERTY()
	AMH_ZoneBase* CurrentZone;

	FString CurrentLevel="";

	// ==================================
	// Widget Interaction ReFactoring
private:
	UPROPERTY(EditDefaultsOnly, Category="UI")
	float WidgetInteractionDistance = 400.f;
	bool bInteracteAnyComponent = false;
	
	void SetWidgetInteractionUsing(bool bUsing);
	bool IsPointingAtWidget();
	void SetClickAndWidgetActivation(bool bUsing);
	void SetWidgetInteractionClick(bool bPress);
	void SetWidgetComponent(bool bSet);

	// 인터렉션 VFX
	UPROPERTY(EditDefaultsOnly)
	UNiagaraComponent* LineTraceEffectComponent;
	UPROPERTY(EditDefaultsOnly)
	float AdditiveTeleportHeight = 10.f;

private:
	// 튜토리얼
	UPROPERTY()
	TObjectPtr<ACTutorialManager> TutorialManager;

	// 앉기 서기
	UPROPERTY(BlueprintReadWrite, meta=(AllowPrivateAccess=true))
	bool bStanding = false;

	// 그랩 수정
	UPROPERTY(EditDefaultsOnly, Category=Grab)
	float GrabRadius = 12;
	UPROPERTY(VisibleAnywhere, Category=Grab)
	TObjectPtr<AActor> RGrabedObject;
	UPROPERTY(VisibleAnywhere, Category=Grab)
	TObjectPtr<AActor> LGrabedObject;
	AActor* GetNearGrabableObject(USceneComponent* GrabController);
	void ObjectGrab(AActor* GrabObject, UMotionControllerComponent* GrabController, bool IsPulling);
	void Drop(UMotionControllerComponent* GrabController); // 박물관 상태 변환용
};
