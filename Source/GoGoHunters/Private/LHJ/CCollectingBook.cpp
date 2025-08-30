#include "LHJ/CCollectingBook.h"

#include "Components/WidgetComponent.h"
#include "Utilities/CHelpers.h"

ACCollectingBook::ACCollectingBook()
{
	PrimaryActorTick.bCanEverTick = true;
	this->Tags.Add("CollectingBook");

	CHelpers::CreateComponent<USceneComponent>(this, &RootScene, "RootScene");
	
	CHelpers::CreateComponent<UWidgetComponent>(this, &CollectingBookWidget, "CollectingBookWidget", RootComponent);
	CollectingBookWidget->SetPivot(FVector2D(0.5, 1.0));
	CollectingBookWidget->SetCollisionProfileName("VRUI");
}

void ACCollectingBook::BeginPlay()
{
	Super::BeginPlay();
}

void ACCollectingBook::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
