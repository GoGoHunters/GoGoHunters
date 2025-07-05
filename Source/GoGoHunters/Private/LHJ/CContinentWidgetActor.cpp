#include "LHJ/CContinentWidgetActor.h"

#include "Components/WidgetComponent.h"
#include "UIs/WorldMap/CContinentWidget.h"
#include "Utilities/CHelpers.h"

ACContinentWidgetActor::ACContinentWidgetActor()
{
	PrimaryActorTick.bCanEverTick = true;
	CHelpers::CreateComponent<USceneComponent>(this, &RootScene, "RootScene");
	CHelpers::CreateComponent<UWidgetComponent>(this, &WidgetComponent, "WidgetComponent", RootComponent);
}

void ACContinentWidgetActor::BeginPlay()
{
	Super::BeginPlay();
	if (WidgetComponent->GetWidget())
		ContinentWidget = Cast<UCContinentWidget>(WidgetComponent->GetWidget());
}

void ACContinentWidgetActor::SetContinentData(const FCContinentData& ContinentData)
{
	if (!ContinentWidget) return;
	ContinentWidget->SetContinentData(ContinentData);
}