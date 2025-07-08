// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/Excavation/ShovelTool.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Blueprint/UserWidget.h"
#include "LHM/Excavation/RelicsGround.h"
#include "Kismet/GameplayStatics.h"

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

	SplatPoint = CreateDefaultSubobject<USceneComponent>(TEXT("SplatPoint"));
	SplatPoint->SetupAttachment(ShovelMesh);
	SplatPoint->SetRelativeRotation(FRotator(18, 0, 3)); // (X=18.000000,Y=0.000000,Z=3.000000)

	bIsDigging = false;
	DiggingRate = 0.1f; // 0.1초마다 한 번씩 데미지 적용
	TimeSinceLastDig = 0.0f;
}

// Called when the game starts or when spawned
void AShovelTool::BeginPlay()
{
	Super::BeginPlay();

	GroundRef = Cast<ARelicsGround>(UGameplayStatics::GetActorOfClass(GetWorld(), ARelicsGround::StaticClass()));
}

// Called every frame
void AShovelTool::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AShovelTool::StartDigging()
{
	bIsDigging = true;
	UE_LOG(LogTemp, Warning, TEXT("Shovel: Digging Started!"));
}

void AShovelTool::StopDigging()
{
	bIsDigging = false;
	UE_LOG(LogTemp, Warning, TEXT("Shovel: Digging Stopped!"));
}
