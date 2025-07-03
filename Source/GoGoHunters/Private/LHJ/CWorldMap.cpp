#include "LHJ/CWorldMap.h"
#include "Utilities/CHelpers.h"

ACWorldMap::ACWorldMap()
{
	PrimaryActorTick.bCanEverTick = true;

	CHelpers::CreateComponent<USceneComponent>(this, &RootScene, "RootScene");
	CHelpers::CreateComponent<UStaticMeshComponent>(this, &Korea, "Korea", RootComponent);
	CHelpers::CreateComponent<UStaticMeshComponent>(this, &NorthAmerica, "NorthAmerica", RootComponent);
	CHelpers::CreateComponent<UStaticMeshComponent>(this, &SouthAmerica, "SouthAmerica", RootComponent);
	CHelpers::CreateComponent<UStaticMeshComponent>(this, &Asia, "Asia", RootComponent);
	CHelpers::CreateComponent<UStaticMeshComponent>(this, &Oceania, "Oceania", RootComponent);
	CHelpers::CreateComponent<UStaticMeshComponent>(this, &Europe, "Europe", RootComponent);
	CHelpers::CreateComponent<UStaticMeshComponent>(this, &Africa, "Africa", RootComponent);

	SetComponentInit(Korea);
	SetComponentInit(NorthAmerica);
	SetComponentInit(SouthAmerica);
	SetComponentInit(Asia);
	SetComponentInit(Oceania);
	SetComponentInit(Europe);
	SetComponentInit(Africa);
}

void ACWorldMap::BeginPlay()
{
	Super::BeginPlay();
}

void ACWorldMap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACWorldMap::SetComponentInit(UStaticMeshComponent* Comp)
{
	Comp->SetCollisionProfileName(FName("Continent"));
	Comp->SetRelativeScale3D(FVector(0.045));
	Comp->SetRelativeLocation(FVector(0, 0, 1));
	Comp->SetCastShadow(false);
}

void ACWorldMap::EnableCompOutline(UStaticMeshComponent* Comp)
{
	if (!Comp) return;

	if (PrevOutlinedComp && PrevOutlinedComp != Comp)
	{
		UMaterialInstanceDynamic* PrevDynMat = Cast<UMaterialInstanceDynamic>(PrevOutlinedComp->GetMaterial(0));
		if (!PrevDynMat)
		{
			PrevDynMat = PrevOutlinedComp->CreateAndSetMaterialInstanceDynamic(0);
		}
		if (PrevDynMat)
		{
			PrevDynMat->SetVectorParameterValue("BC", FLinearColor(0.15f, 0.25f, 1.0f));
		}
	}

	UMaterialInstanceDynamic* DynMat = Cast<UMaterialInstanceDynamic>(Comp->GetMaterial(0));
	if (!DynMat)
	{
		DynMat = Comp->CreateAndSetMaterialInstanceDynamic(0);
	}

	if (DynMat)
	{
		DynMat->SetVectorParameterValue("BC", FLinearColor(1.0f, 0.65f, 0.0f));
		PrevOutlinedComp = Comp;

		// DynMat->SetVectorParameterValue("BC", FLinearColor(0.15f, 0.25f, 1.0f));
		// if (PrevOutlinedComp == Comp)
		// 	PrevOutlinedComp = nullptr;		
	}
}

void ACWorldMap::ResetPrevOutline()
{
	if (PrevOutlinedComp)
	{
		UMaterialInstanceDynamic* DynMat = Cast<UMaterialInstanceDynamic>(PrevOutlinedComp->GetMaterial(0));
		if (!DynMat)
			DynMat = PrevOutlinedComp->CreateAndSetMaterialInstanceDynamic(0);
		if (DynMat)
			DynMat->SetVectorParameterValue("BC", FLinearColor(0.15f, 0.25f, 1.0f));
		PrevOutlinedComp = nullptr;
	}
}
