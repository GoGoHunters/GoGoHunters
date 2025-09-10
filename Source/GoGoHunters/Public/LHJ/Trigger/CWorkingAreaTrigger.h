#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CWorkingAreaTrigger.generated.h"

class UBoxComponent;

USTRUCT(Atomic, BlueprintType)
struct FCNotifyPlayerInTrigger : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	bool bPlayerInTrigger;
	UPROPERTY(BlueprintReadOnly)
	FVector TamiLocation;
};

UCLASS()
class GOGOHUNTERS_API ACWorkingAreaTrigger : public AActor
{
	GENERATED_BODY()

public:
	void SetTemporaryDeactivation(bool InDeactivation) {bTemporaryDeactivation = InDeactivation;}

private:
	UPROPERTY(EditAnywhere)
	UBoxComponent* BoxComponent;
	UPROPERTY(EditAnywhere)
	USceneComponent* AiLocSceneComponent;

	UPROPERTY()
	TObjectPtr<APawn> TamiAI;
	UPROPERTY()
	TObjectPtr<AActor> Player;
	
	UPROPERTY(EditAnywhere, Category="Param|Trigger")
	bool bActiveTrigger = true;
	UPROPERTY(EditAnywhere, Category="Param|Trigger")
	bool bTemporaryDeactivation = false;
	
	ACWorkingAreaTrigger();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION()
	void OnTriggerBeginOverrlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnTriggerEndOverrlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	FVector GetSceneCompLocation();
};
