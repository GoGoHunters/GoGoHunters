// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/Excavation/ShovelTool.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/BoxComponent.h"

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
	}

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	BoxComp->SetupAttachment(ShovelMesh);
	BoxComp->SetRelativeRotation(FRotator(10, 0, 0)); // (Pitch=10.000000,Yaw=0.000000,Roll=0.000000)
	BoxComp->SetBoxExtent(FVector(17, 17, 3.5)); // (X=17.000000,Y=17.000000,Z=3.500000)

	//ShovelMesh->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	//ShovelMesh->SetSimulatePhysics(true);
	
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

