// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/Restore/PieceActor.h"
#include "LHJ/Pickup/CRelicPickupActorComponent.h"

// Sets default values
APieceActor::APieceActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APieceActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APieceActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APieceActor::DestroyPickupComp()
{  
    if (UActorComponent* PickupComp = FindComponentByClass<UCRelicPickupActorComponent>())
    {
        PickupComp->DestroyComponent();
    }
}

