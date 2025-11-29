// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RelicsGround.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGroundDugDelegate, FVector, ImpactLocation);

UCLASS()
class GOGOHUNTERS_API ARelicsGround : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARelicsGround();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UFUNCTION(BlueprintCallable, Category = "Diggind Events")
	void SetShovelReference(class AShovelTool* NewShovelRef);
	UFUNCTION(BlueprintCallable, Category = "Diggind Events")
	void SetRelicsManager(class ARelicsManager* NewRelicsManager) { RelicsManager = NewRelicsManager; }

	void ClearShovelReference();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Digging Events")
	class AShovelTool* Shovel_Ref;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Digging Events")
	class ARelicsManager* RelicsManager;

public:
	UFUNCTION(BlueprintCallable, Category = "Digging Events")
	void TriggerOnGroundDug(FVector ImpactLocation);

	UPROPERTY(BlueprintAssignable, Category = "Digging Events")
	FOnGroundDugDelegate OnGroundDug;

	void UpdateDigProgress();
	float CalculateDestructionFromRenderTarget();

	UFUNCTION(BlueprintCallable, Category = "Digging Events")
	void SetPainterMaterial(class UMaterialInstanceDynamic* InMaterial) { PainterMaterial = InMaterial; }
	
	UFUNCTION(BlueprintCallable)
	void SetHeightFieldRenderTarget(UTextureRenderTarget2D* InRT) { HeightFieldRT = InRT; }

	UPROPERTY()
	class UMaterialInstanceDynamic* PainterMaterial;
	
	UPROPERTY()
	UTextureRenderTarget2D* HeightFieldRT;
};
