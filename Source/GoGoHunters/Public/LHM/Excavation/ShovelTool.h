// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShovelTool.generated.h"

UCLASS()
class GOGOHUNTERS_API AShovelTool : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShovelTool();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ShovelTool")
	class UStaticMeshComponent* ShovelMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ShovelTool")
	class USceneComponent* SplatPoint;
	
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Digging")
    bool bIsDigging;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Digging")
    float DiggingRate; // 초당 삽질 적용 빈도 (0.1초마다 한 번)

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Digging")
    float TimeSinceLastDig; // 마지막으로 삽질 데미지를 적용한 이후 경과 시간

public:
    UFUNCTION(BlueprintCallable, Category = "Digging")
	void SetIsDigging(bool bNewIsDigging) { bIsDigging = bNewIsDigging; }

	UFUNCTION()
	void UpdateFeedback(FVector ImpactLocation);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Digging")
	class UHapticFeedbackEffect_Base* DigHapticEffect;
};
