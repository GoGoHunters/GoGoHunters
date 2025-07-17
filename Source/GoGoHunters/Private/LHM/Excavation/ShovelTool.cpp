// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/Excavation/ShovelTool.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Blueprint/UserWidget.h"
#include "LHM/Excavation/RelicsGround.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"

// Sets default values
AShovelTool::AShovelTool()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	ShovelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShovelMesh"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ShovelMeshAsset(TEXT("/Game/LHM/Meshes/SM_Shovel.SM_Shovel"));
	if (ShovelMeshAsset.Succeeded())
	{
		ShovelMesh->SetStaticMesh(ShovelMeshAsset.Object);
		ShovelMesh->SetupAttachment(RootComponent);
		ShovelMesh->SetRelativeLocation(FVector(90, 0, 0)); // (X=80.000000,Y=0.000000,Z=0.000000)
	}

	SplatPoint = CreateDefaultSubobject<USceneComponent>(TEXT("SplatPoint"));
	SplatPoint->SetupAttachment(ShovelMesh);
	SplatPoint->SetRelativeLocation(FVector(15, 0, 10)); // (X=15.000000,Y=0.000000,Z=10.000000)

	bIsDigging = false;
	DiggingRate = 0.1f; // 0.1초마다 한 번씩 데미지 적용
	TimeSinceLastDig = 0.0f;
}

// Called when the game starts or when spawned
void AShovelTool::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AShovelTool::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AShovelTool::PlayFeedback(FVector ImpactLocation)
{
	if (!bIsDigging) return;

	// 햅틱 피드백 재생
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC && DigHapticEffect)
	{ 
		PC->PlayHapticEffect(DigHapticEffect, EControllerHand::Right, 1.0f, false);
	}
}
