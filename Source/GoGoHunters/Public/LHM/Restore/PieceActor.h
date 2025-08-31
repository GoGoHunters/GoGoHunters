// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PieceActor.generated.h"

UCLASS()
class GOGOHUNTERS_API APieceActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APieceActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	void SetPieceIndex(int32 Index) { PieceIndex = Index; }
	int32 GetPieceIndex() const { return PieceIndex; }

	void SetSnapped(bool b) { bSnapped = b; }
	bool IsSnapped() const { return bSnapped; }

private:
	UPROPERTY(VisibleAnywhere)
	int32 PieceIndex = -1;

	UPROPERTY(VisibleAnywhere)
	bool bSnapped = false;

};
