// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ZoneDetectorComp.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnZoneDetectedSignature, AActor*, ZoneActor);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GOGOHUNTERS_API UZoneDetectorComp : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UZoneDetectorComp();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//체크 호출
	UFUNCTION(BlueprintCallable)
	void CheckZone();

	//감지되면 호출 될 이벤트
	UPROPERTY(BlueprintAssignable)
	FOnZoneDetectedSignature OnZoneDetected;


};
