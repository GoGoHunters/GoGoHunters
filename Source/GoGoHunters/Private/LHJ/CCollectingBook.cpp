#include "LHJ/CCollectingBook.h"

#include "Components/WidgetComponent.h"
#include "UIs/CollectingBook/CCollectingBookWidget.h"
#include "Utilities/CHelpers.h"

ACCollectingBook::ACCollectingBook()
{
	PrimaryActorTick.bCanEverTick = true;
	this->Tags.Add("CollectingBook");

	CHelpers::CreateComponent<USceneComponent>(this, &RootScene, "RootScene");

	CHelpers::CreateComponent<UWidgetComponent>(this, &CollectingBookWidget, "CollectingBookWidget", RootComponent);
	CollectingBookWidget->SetPivot(FVector2D(0.5, 1.0));
	CollectingBookWidget->SetCollisionProfileName("VRUI");

	this->SetActorEnableCollision(false);
}

void ACCollectingBook::BeginPlay()
{
	Super::BeginPlay();

	CollectingBook = Cast<UCCollectingBookWidget>(CollectingBookWidget->GetWidget());
}

void ACCollectingBook::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bStartPlayerDistCheck)
	{
		APawn* Pawn = GetWorld()->GetFirstPlayerController()->GetPawn();
		float Distance = FVector::Dist(this->GetActorLocation(), Pawn->GetActorLocation());
		UE_LOG(LogTemp, Warning, TEXT("Distance : %f"), Distance);
		if (Distance > Dist)
		{
			ActiveAnim(false);
			bStartPlayerDistCheck = false;
		}
	}
}
