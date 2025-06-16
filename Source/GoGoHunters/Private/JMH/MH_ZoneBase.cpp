// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_ZoneBase.h"

// Sets default values
AMH_ZoneBase::AMH_ZoneBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMH_ZoneBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMH_ZoneBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

