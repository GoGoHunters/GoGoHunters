// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/Excavation/TweezersTool.h"

// Sets default values
ATweezersTool::ATweezersTool()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));

	TweezersMeshL = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TweezersMeshL"));
	TweezersMeshR = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TweezersMeshR"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAssets(TEXT("/Game/LHM/Meshes/SM_Tweezerses.SM_Tweezerses"));
	if (MeshAssets.Succeeded())
	{
		TweezersMeshL->SetStaticMesh(MeshAssets.Object);
		TweezersMeshL->SetupAttachment(RootComponent);
		TweezersMeshL->SetRelativeLocation(FVector(25, -2, 10)); // (X=25.000000,Y=-2.000000,Z=10.000000)
		TweezersMeshL->SetRelativeRotation(FRotator(0, -5, 0)); // (Pitch=0.000000,Yaw=-5.000000,Roll=-0.000000)

		TweezersMeshR->SetStaticMesh(MeshAssets.Object);
		TweezersMeshR->SetupAttachment(RootComponent);
		TweezersMeshR->SetRelativeLocation(FVector(25, 2, 10)); // (X=25.000000,Y=2.000000,Z=10.000000)
		TweezersMeshR->SetRelativeRotation(FRotator(0, 5, 0)); // (Pitch=0.000000,Yaw=5.000000,Roll=-0.000000)
	}
}

// Called when the game starts or when spawned
void ATweezersTool::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATweezersTool::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(bIsPickingUp) PickUpTweezers();
}

void ATweezersTool::PickUpTweezers()
{
	//UE_LOG(LogTemp, Log, TEXT("[TweezersTool] PickUpTweezers"));
}

void ATweezersTool::SetIsPickingUp(bool _bIsPickingUp)
{
	bIsPickingUp = _bIsPickingUp;

	UE_LOG(LogTemp, Log, TEXT("[TweezersTool] SetIsPickingUp: %s"), bIsPickingUp ? TEXT("True") : TEXT("False"));

	if (bIsPickingUp)
	{
		TweezersMeshL->SetRelativeLocation(FVector(25, -1, 10));
		TweezersMeshL->SetRelativeRotation(FRotator(0, 0, 0));
		TweezersMeshR->SetRelativeLocation(FVector(25, 1, 10));
		TweezersMeshR->SetRelativeRotation(FRotator(0, 0, 0));
	}
	else
	{
		TweezersMeshL->SetRelativeLocation(FVector(25, -2, 10));
		TweezersMeshL->SetRelativeRotation(FRotator(0, -5, 0));
		TweezersMeshR->SetRelativeLocation(FVector(25, 2, 10));
		TweezersMeshR->SetRelativeRotation(FRotator(0, 5, 0));
	}
}

