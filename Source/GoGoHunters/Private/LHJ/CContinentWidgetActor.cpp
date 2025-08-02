#include "LHJ/CContinentWidgetActor.h"

#include "Components/WidgetComponent.h"
#include "LHJ/CWorldMap.h"
#include "UIs/WorldMap/CContinentWidget.h"
#include "Utilities/CHelpers.h"

ACContinentWidgetActor::ACContinentWidgetActor()
{
	PrimaryActorTick.bCanEverTick = true;
	CHelpers::CreateComponent<USceneComponent>(this, &RootScene, "RootScene");
	CHelpers::CreateComponent<UWidgetComponent>(this, &WidgetComponent, "WidgetComponent", RootComponent);
	WidgetComponent->SetCollisionProfileName(FName("VRUI"));
}

void ACContinentWidgetActor::BeginPlay()
{
	Super::BeginPlay();
	
	if (WidgetComponent->GetWidget())
	{
		ContinentWidget = Cast<UCContinentWidget>(WidgetComponent->GetWidget());
		ContinentWidget->SetOuterActor(this);
	}
}

void ACContinentWidgetActor::SetContinentData(const FCContinentData& ContinentData)
{
	if (!ContinentWidget) return;
	ContinentWidget->SetContinentData(ContinentData);
}

void ACContinentWidgetActor::SetContinentVisibleHidden()
{
	if (!OuterOwner) return;
	OuterOwner->SetContinentVisibleHidden();
}

void ACContinentWidgetActor::SetOuterActor(ACWorldMap* InOwner)
{
	if (!InOwner) return;
	OuterOwner = InOwner;
}
