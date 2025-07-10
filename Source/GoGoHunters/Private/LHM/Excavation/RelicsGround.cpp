// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/Excavation/RelicsGround.h"
//#include "Engine/TextureRenderTarget2D.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Kismet/GameplayStatics.h"
#include "LHM/Excavation/ShovelTool.h"
//#include "Kismet/KismetRenderingLibrary.h"

// Sets default values
ARelicsGround::ARelicsGround()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ARelicsGround::BeginPlay()
{
	Super::BeginPlay();

	SetActorHiddenInGame(true);
}

// Called every frame
void ARelicsGround::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARelicsGround::SetShovelReference(class AShovelTool* NewShovelRef)
{
	Shovel_Ref = NewShovelRef;

	if (Shovel_Ref) OnGroundDug.AddDynamic(Shovel_Ref, &AShovelTool::PlayFeedback);
}

void ARelicsGround::TriggerOnGroundDug(FVector ImpactLocation)
{
	OnGroundDug.Broadcast(ImpactLocation);
}

