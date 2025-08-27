#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MH_GrabComp.generated.h"


/*
 * 물체를 손으로 잡는 컴포넌트 (기능만 담당)
 * - 라인 트레이스 및 대상 판단은 외부(Player)가 수행
 * - 이 컴포넌트는 단순히 Grab / Release / Rotate 기능만 수행
 * - BeginPlay에서 손 위치를 지정하는 HandComponent를 외부에서 할당해야 함
 */

class AMH_VRPlayer;
class UCMuseumComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GOGOHUNTERS_API UMH_GrabComp : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMH_GrabComp();
	bool SetGrab(UPrimitiveComponent* GrabComp);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	bool TryGrab(UPrimitiveComponent* TargetComp);
	void RelicGrab(AActor* Relic);
	void TryUnGrab();
	void RelicUnGrab(AActor* Relic);
	void ReleaseGrabbedComponent();

	UPROPERTY(EditAnywhere, Category="Grab")
	float GrabPullSpeed = 5.0f;

	UPROPERTY()
	TObjectPtr<AMH_VRPlayer> OwnerPlayer = nullptr;
	UPROPERTY()
	TObjectPtr<UCMuseumComponent> MuseumComponent = nullptr;
	
	//손 위치
	UPROPERTY()
	USceneComponent* HandComponent = nullptr;

	// 상태
	//잡고있는 상태
	UPROPERTY()
	bool bIsGrabbing = false;
	//손으로 오고있는 상태
	UPROPERTY()
	bool bIsPulling = false;

	// 잡을 대상
	UPROPERTY()
	UPrimitiveComponent* PendingGrabComponent = nullptr;
	//현재 잡고있는 대상
	UPROPERTY()
	UPrimitiveComponent* GrabbedComponent = nullptr;
	//손 컴포넌트를 설정(플레이어 BeginPlay 등에서 호출)
	void SetHandComponent(USceneComponent* NewHand) { HandComponent = NewHand; }

	//물체 회전
	void RotateGrabbedObject(const FVector2D& Input);

	UPROPERTY(EditAnywhere)
	float HeldObjectRotateSpeed = 100.f;

	UFUNCTION(BlueprintPure)
	bool IsGrabbing() const { return bIsGrabbing && GrabbedComponent != nullptr; }
};
