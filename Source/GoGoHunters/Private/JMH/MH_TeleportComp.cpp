// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_TeleportComp.h"

#include "NiagaraComponent.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"

// Sets default values for this component's properties
UMH_TeleportComp::UMH_TeleportComp()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UMH_TeleportComp::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UMH_TeleportComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	PrimaryComponentTick.bCanEverTick = true;

}

void UMH_TeleportComp::SetHandComponent(USceneComponent* HandComp)
{
	Hand = HandComp;
}

void UMH_TeleportComp::SetTeleportVisual(UNiagaraComponent* Circle, UNiagaraComponent* UI)
{
	TeleportCircle = Circle;
	TeleportUI = UI;
}

void UMH_TeleportComp::EnableTeleport()
{
	if (TeleportUI)
		TeleportUI->SetVisibility(true);

	bTeleporting = true;
}

bool UMH_TeleportComp::CompleteTeleport(FVector& OutLocation)
{
	if (!bTeleporting || !TeleportCircle) return false;

	bool bSuccess = bCanTeleport;
	TeleportCircle->SetVisibility(false);
	if (TeleportUI) TeleportUI->SetVisibility(false);

	bTeleporting = false;

	if (bSuccess)
	{
		OutLocation = TeleportLocation;
	}

	return bSuccess;
}

/*
void UMH_TeleportComp::DrawTeleportStraight()
{
	FVector Start = Hand->GetComponentLocation();
	FVector End = Start + Hand->GetForwardVector() * 1000.f;

	Lines.Add(Start);
	Lines.Add(End);
}*/

void UMH_TeleportComp::UpdateTargetLocation(const FVector& NewLocation)
{
	TeleportLocation = NewLocation;
	bCanTeleport = true;

	if (TeleportCircle)
	{
		TeleportCircle->SetVisibility(true);
		TeleportCircle->SetWorldLocation(NewLocation);
	}
}

void UMH_TeleportComp::SetInvalidTeleport()
{
	bCanTeleport = false;

	if (TeleportCircle)
	{
		TeleportCircle->SetVisibility(false);
	}
}
