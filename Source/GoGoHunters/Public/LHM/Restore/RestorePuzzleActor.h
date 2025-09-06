// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LHJ/CRelicData.h"
#include "RestorePuzzleActor.generated.h"

UCLASS()
class GOGOHUNTERS_API ARestorePuzzleActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARestorePuzzleActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void InitPuzzle(const FCRelicData& InRelicData, class ARestoreManager* InManager);

	UFUNCTION()
	void TrySnapPiece(class APieceActor* Piece);

	void CheckPuzzleCompleted();
	void OnPuzzleCompleted();

	const FCRelicData& GetRelicData() const { return RelicData; }
	TArray<class AActor*> GetPieceActors() const { return PieceActors; }

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> RelicClass;

	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* RotationBoard;


	UPROPERTY()
	AActor* SpawnedRelic;

	UPROPERTY()
	class ARestoreManager* RestoreManager;

	UPROPERTY()
	TArray<USceneComponent*> SnapPoints;

	UPROPERTY()
	TArray<AActor*> PieceActors;

private:
	FCRelicData RelicData;

	UPROPERTY()
	UStaticMeshComponent* GuideMesh;

	void PlayFeedback(bool bSuccess);

	// Feedback
	UPROPERTY(EditAnywhere, Category = "Feedback")
	class USoundBase* SuccessSFX = nullptr;
	
	UPROPERTY(EditAnywhere, Category = "Feedback")
	class USoundBase* FailSFX = nullptr;

	UPROPERTY(EditAnywhere, Category = "Feedback")
	class UHapticFeedbackEffect_Base* FailHaptic = nullptr;

};
