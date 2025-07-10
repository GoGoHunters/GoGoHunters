// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/Excavation/RelicsBase.h"
#include "LHM/Excavation/ExcavationMarker.h"

// Sets default values
ARelicsBase::ARelicsBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ARelicsBase::BeginPlay()
{
	Super::BeginPlay();
	
    if (MarkerClass)
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
        Marker = GetWorld()->SpawnActor<AExcavationMarker>(MarkerClass, GetActorLocation(), GetActorRotation(), SpawnParams);

        if (Marker)
        {
            Marker->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
            Marker->SetActorLocation(GetActorLocation()+FVector(0,0,230));
            Marker->SetActorRelativeScale3D(FVector(2.5f));
            Marker->SetActorHiddenInGame(true);
        }
    }
}

// Called every frame
void ARelicsBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

