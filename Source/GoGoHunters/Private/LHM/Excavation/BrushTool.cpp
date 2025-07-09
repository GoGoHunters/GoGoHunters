// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/Excavation/BrushTool.h"

// Sets default values
ABrushTool::ABrushTool()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	BrushMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BrushMesh"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Game/LHM/Meshes/SM_Brush.SM_Brush"));
	if (MeshAsset.Succeeded())
	{
		BrushMesh->SetStaticMesh(MeshAsset.Object);
		BrushMesh->SetupAttachment(RootComponent);
	}
}

// Called when the game starts or when spawned
void ABrushTool::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABrushTool::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

