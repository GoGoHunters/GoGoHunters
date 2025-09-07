// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ArrowComponent.h"
#include "AC_LineTrace.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHitDetected, AActor*, HitActor);

UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GOGOHUNTERS_API UAC_LineTrace : public UArrowComponent
{
	GENERATED_BODY()

public:
	UAC_LineTrace();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LineTrace")
	float TraceDistance = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LineTrace")
	bool bShowDebugTrace = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LineTrace")
	FName SocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LineTrace")
	USceneComponent* ParentComponent;

	UFUNCTION(BlueprintCallable, Category = "Socket Attachment")
	bool AttachToSocket(USceneComponent* InParentComponent, FName InSocketName);

	UPROPERTY(BlueprintAssignable, Category = "Line Trace")
	FOnHitDetected OnHitDetected;

	UFUNCTION(BlueprintCallable, Category = "LineTrace")
	void PerformLineTrace();
};