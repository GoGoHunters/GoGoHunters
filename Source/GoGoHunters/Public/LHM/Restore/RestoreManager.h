// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LHJ/CRelicData.h"
#include "RestoreManager.generated.h"

UCLASS()
class GOGOHUNTERS_API ARestoreManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARestoreManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	void StartRestoration(const FCRelicData& RelicData);
	void SpawnPuzzleActor(const FCRelicData& RelicData);

	UFUNCTION()
	void NotifyPuzzleCompleted(class ARestorePuzzleActor* PuzzleActor);

	void SetRestoreUI(class URestoreUI* InRestoreUI) { RestoreUI = InRestoreUI; }

private:
	UPROPERTY()
	TObjectPtr<class URestoreUI> RestoreUI;

	//UPROPERTY(EditAnywhere, Category = "Puzzle")
	//TSubclassOf<class ARestorePuzzleActor> PuzzleActorClass;

	//UPROPERTY()
	//TObjectPtr<class ARestorePuzzleActor> ActivePuzzleActor;

	UPROPERTY(EditInstanceOnly)
	class ARestorePuzzleActor* ActivePuzzleActor;

	UPROPERTY()
	FCRelicData CurrentRelicData;
};
