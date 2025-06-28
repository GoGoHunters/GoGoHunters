// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputAction.h"
#include "MH_VRPlayer.generated.h"

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


	//Grab////////////////////////////////////////////////////////////////////

	// Grab 잡을 범위
	UPROPERTY(EditAnywhere, Category="Grab")
	float GrabRadius = 100.f;

	UPROPERTY()
	UPrimitiveComponent* grabbedObject = nullptr;

	//물체를 잡고있는지 여부
	bool bIsGrabbing = false;
	UPROPERTY()
	AActor* FocusedGrabbableActor;

	//grab 한 물체 손으로 당겨오기
	UPrimitiveComponent* PendingGrabComponent = nullptr;
	bool bIsPullingObject = false;
	float GrabPullSpeed = 10.f;
	// 회전 속도 조절
	UPROPERTY(EditAnywhere, Category = "Grab")
	float HeldObjectRotateSpeed = 100.f;

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

	//텔레포트 곡선 방식
	//곡선을 이루는 점의 개수(곡선의 부드러운 정도)
	UPROPERTY(EditAnywhere, Category="Teleport")
	int32 LineSmooth = 40;
	//Curve 를 그리며 날아가는 힘의 세기
	UPROPERTY(EditAnywhere, Category="Teleport")
	float CurveForce = 2000;
	//중력가속도
	UPROPERTY(EditAnywhere, Category="Teleport")
	float Gravity = -5000;
	//Delta time
	UPROPERTY(EditAnywhere, Category="Teleport")
	float SimulateTime = 0.02f;

	//텔레포트 라인이동 최대거리, 속도
	UPROPERTY(EditAnywhere, Category = "Teleport")
	float MaxTeleportDistance = 3000.f;
	UPROPERTY(EditAnywhere, Category = "Teleport")
	float TeleportAdjustSpeed = 600.f;

	//텔레포트 구역 원
	UPROPERTY(VisibleAnywhere)
	class UNiagaraComponent* TeleportCircleA;

	//텔레포트 나이아가라
	UPROPERTY(VisibleAnywhere)
	class UNiagaraComponent* TeleportUIComponent;

	//텔레포트 진행여부
	bool bTeleporting = false;
	bool bIsDebugDraw = true;
	bool bCanTeleportLocationValid = false;
	//텔레포트 위치
	FVector TeleportLocation;
	//기억할 점 리스트
	TArray<FVector> Lines;
	//텔레포트 모드 전환 (Curve로 할지 직선으로 할지)
	UPROPERTY(EditAnywhere, Category="Teleport")
	bool bTeleportCurve = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TeleportDistanceFactor = 1.0f; // 0~1 범위로 라인 길이 비율 제어
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TeleportDistanceAdjustSpeed = 1.0f; // 입력에 따른 조정 속도
	
	//키 이벤트 바인딩 함수
	void F_TeleportStart(const struct FInputActionValue& Value);
	void F_TeleportEnd(const struct FInputActionValue& Value);
	//텔레포트 초기화 함수
	UFUNCTION()
	bool ResetTeleport();
	UFUNCTION()
	bool CheckHitTeleport(FVector LastPos, FVector& CurPos);
	//직선 텔레포트 그리기
	void DrawTeleportStraight();
	//곡선 텔레포트 그리기
	void DrawTeleportCurve();
	void AdjustTeleportDirection(const FInputActionValue& Value);
	//텔레포트 or Grab
	void HandleThumbstickInput(const FInputActionValue& Value);
};
