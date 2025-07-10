// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExcavationManager.generated.h"

UCLASS()
class GOGOHUNTERS_API AExcavationManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AExcavationManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	// 탐지된 유물에 대한 발굴 시작 지시
	void StartRelicsExcavation(class ARelicsManager* Target);

	// 붓단계 진입 콜백 (RelicsManager → 통보)
	void OnBrushPhaseEntered(class ARelicsManager* Target);

protected:
	UPROPERTY()
	TArray<class ARelicsManager*> AllRelicsManagers;

	UPROPERTY()
	class ARelicsManager* CurrentActiveManager;
};
