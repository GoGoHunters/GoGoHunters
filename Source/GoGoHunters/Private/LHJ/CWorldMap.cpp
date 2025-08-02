#include "LHJ/CWorldMap.h"
#include "LHJ/CContinentData.h"
#include "LHJ/CContinentWidgetActor.h"
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
	CHelpers::CreateComponent<UChildActorComponent>(this, &ContinentWidget, "ContinentWidget", RootComponent);

	SetComponentInit(Korea);
	SetComponentInit(NorthAmerica);
	SetComponentInit(SouthAmerica);
	SetComponentInit(Asia);
	SetComponentInit(Oceania);
	SetComponentInit(Europe);
	SetComponentInit(Africa);

	ContinentWidget->SetVisibility(false);
}

void ACWorldMap::BeginPlay()
{
	Super::BeginPlay();
	if (ContinentWidget && ContinentWidget->GetChildActor())
	{
		ContinentWidgetActor = Cast<ACContinentWidgetActor>(ContinentWidget->GetChildActor());
		ContinentWidgetActor->SetOuterActor(this);
	}

	if (ContinentDataTable)
	{
		TArray<FCContinentData*> AllItems;
		ContinentDataTable->GetAllRows<FCContinentData>(TEXT("None Data"), AllItems);
		for (FCContinentData* Item : AllItems)
		{
			ContinentDataArray.Add(*Item);
		}
	}
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
	}
	
	if (ContinentWidgetActor)
	{
		FCContinentData ContinentData = GetContinentData(Comp->GetName());
		if (ContinentData.ContinentName.IsEmpty()) return;
		if (!ContinentWidget->IsVisible()) ContinentWidget->SetVisibility(true);
		ContinentWidgetActor->SetContinentData(ContinentData);
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

void ACWorldMap::SetContinentVisibleHidden()
{
	if (ContinentWidget && ContinentWidget->IsVisible()) ContinentWidget->SetVisibility(false);
}

const FCContinentData ACWorldMap::GetContinentData(const FString& ContinentName)
{
	for (FCContinentData& Data : ContinentDataArray)
	{
		if (Data.Key == ContinentName)
			return Data;
	}

	return FCContinentData();
}
