// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/Excavation/ExcavationMarker.h"
#include "../../../../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraFunctionLibrary.h"

// Sets default values
AExcavationMarker::AExcavationMarker()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AExcavationMarker::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AExcavationMarker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AExcavationMarker::ActivateMarker()
{
	SetActorHiddenInGame(false);
	
	// 이펙트, 깃발 표시 등
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), PulseEffect, GetActorLocation());
	
	// 머티리얼에 하이라이트 효과 추가

}

