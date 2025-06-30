// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputAction.h"
#include "MH_VRPlayer.generated.h"


UENUM(BlueprintType)
enum class EPlayerVRState : uint8
{
	Idle			UMETA(DisplayName = "Idle"),			// 아무것도 하지 않는 기본 상태
	UsingTool		UMETA(DisplayName = "Using Tool"),		// 도구를 손에 든 상태 (아직 사용은 안함)
	Excavating		UMETA(DisplayName = "Excavating"),		// 도구를 실제로 사용하여 발굴 중
	GrabbingObject	UMETA(DisplayName = "Grabbing Object"),	// 유물(또는 도구 등)을 손에 잡은 상태
	Inspecting		UMETA(DisplayName = "Inspecting"),		// 손에 든 유물/오브젝트를 관찰(회전/확대) 중
	PlacingObject	UMETA(DisplayName = "Placing Object"),	// 나만의 박물관에서 오브젝트를 배치 중
	Teleporting		UMETA(DisplayName = "Teleporting"),	
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

	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* VRCamera;
	//손일단 VR 카메라에 Root 붙여 놓음 이동해야함 (수정)
	UPROPERTY(VisibleAnywhere)
	class USceneComponent* L_Hand;
	UPROPERTY(VisibleAnywhere)
	class USceneComponent* R_Hand;

	// 현재 플레이어의 상태
	UPROPERTY(BlueprintReadWrite, Category = "State")
	EPlayerVRState CurrentState = EPlayerVRState::Idle;
	
	UFUNCTION(BlueprintCallable, Category = "State")
	void SetPlayerState(EPlayerVRState NewState);

	UFUNCTION(BlueprintPure, Category = "State")
	EPlayerVRState GetPlayerState() const;

	UPROPERTY()
	AActor* FocusedGrabbableActor;

public:
	
	//IA////////////////////////////////////////////////////////
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	class UInputMappingContext* InputMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* IA_MHGrab;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* IA_MHInteract;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* IA_MHTeleportEnd;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* IA_MHTeleportStart;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* IA_MHTurn;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* IA_MHLookUp;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* IA_AdjustTeleportDirection;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* IA_RotateHeldObject;

	//마우스 회전방지
	bool bUseMouse = true;

	//Line Trace
	void UpdateInteractionLine();

	//Grab////////////////////////////////////////////////////////////////////

	UPROPERTY()
	class UMH_GrabComp* GrabComponent;
	
	// 잡기 함수들
	void TryGrab(const struct FInputActionValue& Value);
	void TryUnGrab(const struct FInputActionValue& Value);
	
	// 썸스틱 입력 저장
	FVector2D HeldObjectStickInput = FVector2D::ZeroVector;

	//test Key
	void TestTurn(const FInputActionValue& Value);
	void TestLookUp(const FInputActionValue& Value);
	UFUNCTION()
	void TestInteract();
	
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
};
