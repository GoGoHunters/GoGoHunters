#pragma once

#include "CoreMinimal.h"
#include "LHJ/Pickup/CPickupActorComponent.h"
#include "CRelicPickupActorComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GOGOHUNTERS_API UCRelicPickupActorComponent : public UCPickupActorComponent
{
	GENERATED_BODY()

public:
	void Pickup(USceneComponent* AttachTo, bool IsPulling) override;
	void Drop(USceneComponent* DropFrom) override;
	
private:
	// Drop 경로에서의 간접 호출을 구분하기 위한 플래그
	bool bInvokedFromDrop = false;
	
	UCRelicPickupActorComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GrabOverrideFunc() override;
	virtual void ReleaseOverrideFunc() override;	
};
