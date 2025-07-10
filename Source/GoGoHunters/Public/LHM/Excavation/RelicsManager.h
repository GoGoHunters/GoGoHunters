// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RelicsManager.generated.h"

UCLASS()
class GOGOHUNTERS_API ARelicsManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARelicsManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	void StartExcavation(); // 초기 시작
	void NotifyGroundProgress(float Progress); // HeightMap 파괴량 보고

	UFUNCTION(BlueprintCallable)
	class ARelicsBase* GetRelics() const { return Relics; }

protected:
	UPROPERTY(VisibleAnywhere)
	TArray<class UChildActorComponent*> GroundChildActors;

	UPROPERTY(VisibleAnywhere)
	class UChildActorComponent* RelicsChild;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* ExcavationLand_01;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* ExcavationLand_02;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* ExcavationSite;

private:
	TArray<class ARelicsGround*> GroundLayers;

	UPROPERTY(EditAnywhere, Category="Relics")
	TSubclassOf<ARelicsBase> RelicsClass;

	class ARelicsBase* Relics;

	int32 CurrentLayerIndex = 0;
	bool bBrushPhaseStarted = false;

	void EnterBrushPhase();
};
