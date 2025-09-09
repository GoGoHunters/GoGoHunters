// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExcavationWidgetActor.generated.h"

UCLASS()
class GOGOHUNTERS_API AExcavationWidgetActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AExcavationWidgetActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetCollisionAndVisible(bool InVisible);

private:
	UPROPERTY(EditDefaultsOnly)
    class UWidgetComponent* WidgetComponent;

	bool bIsVisible = false;

};
