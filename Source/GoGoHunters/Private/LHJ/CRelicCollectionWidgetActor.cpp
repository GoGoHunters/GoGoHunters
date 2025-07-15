#include "LHJ/CRelicCollectionWidgetActor.h"

#include "Components/WidgetComponent.h"
#include "UIs/Museum/CRelicCollectionWidget.h"
#include "Utilities/CHelpers.h"

ACRelicCollectionWidgetActor::ACRelicCollectionWidgetActor()
{
	PrimaryActorTick.bCanEverTick = true;
	CHelpers::CreateComponent<USceneComponent>(this, &RootScene, "RootScene");
	CHelpers::CreateComponent<UWidgetComponent>(this, &WidgetComponent, "WidgetComponent", RootComponent);
	WidgetComponent->SetCollisionProfileName(FName("VRUI"));
	WidgetComponent->SetRelativeRotation(FRotator(0.f, 180.f, 0.f));
	WidgetComponent->SetRelativeScale3D(FVector(1.f, .1f, .1f));
}

void ACRelicCollectionWidgetActor::BeginPlay()
{
	Super::BeginPlay();
	if (WidgetComponent->GetWidget())
 		RelicCollectionWidget = Cast<UCRelicCollectionWidget>(WidgetComponent->GetWidget());
}

void ACRelicCollectionWidgetActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACRelicCollectionWidgetActor::ReloadRelicList()
{
	if (!RelicCollectionWidget) return;
	RelicCollectionWidget->InitRelicWidgets();
}
