// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MH_TeleportComp.generated.h"

/*
 * 플레이어의 텔레포트 계산 및 시각화 전용 컴포넌트
 * - 외부에서 손 위치 및 방향을 설정해야 함 (SetHandComponent)
 * - 라인 그리기 및 충돌 계산을 통해 이동 가능 위치를 결정
 * - 실제 입력 처리나 Focus 판단은 외부(Player)가 담당
 */

class UNiagaraComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GOGOHUNTERS_API UMH_TeleportComp : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMH_TeleportComp();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// 외부에서 손 위치, 방향 설정
	void SetHandComponent(USceneComponent* HandComp);
	
	// 텔레포트 UI 구성 요소 설정
	void SetTeleportVisual(UNiagaraComponent* Circle, UNiagaraComponent* UI);

	// 텔레포트 라인 계산 방식
	void EnableTeleport();
	bool CompleteTeleport(FVector& OutLocation);
	bool IsTeleporting() const { return bTeleporting; }
	
	// 라인 결과 접근
	const TArray<FVector>& GetLines() const { return Lines; }


private:
	// 시각 요소
	UPROPERTY()
	UNiagaraComponent* TeleportCircle;

	UPROPERTY()
	UNiagaraComponent* TeleportUI;

	UPROPERTY()
	USceneComponent* Hand;

	UPROPERTY()
	TArray<FVector> Lines;

	UPROPERTY()
	bool bTeleporting = false;

	UPROPERTY()
	bool bUseCurve = true;

	UPROPERTY()
	bool bCanTeleport = false;

	UPROPERTY()
	FVector TeleportLocation;

	// 조정 값
	UPROPERTY(EditAnywhere)
	int32 LineSmooth = 40;

	UPROPERTY(EditAnywhere)
	float CurveForce = 2000.f;

	UPROPERTY(EditAnywhere)
	float Gravity = -5000.f;

	UPROPERTY(EditAnywhere)
	float SimulateTime = 0.02f;

	UPROPERTY(EditAnywhere)
	float TeleportAdjustSpeed = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Teleport")
	float TeleportDistanceAdjustSpeed = 1.0f;

public:
	// 외부에서 텔레포트 위치를 강제로 지정 (라인 트레이스 방식용)
	void UpdateTargetLocation(const FVector& NewLocation);

	void SetInvalidTeleport();
};
