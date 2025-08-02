#include "UIs/CUiActor.h"

#include "JMH/CMuseumComponent.h"
#include "JMH/MH_VRPlayer.h"
#include "UIs/CWidgetBase.h"
#include "UIs/Museum/CRelicCollectionWidget.h"

ACUiActor::ACUiActor()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ACUiActor::BeginPlay()
{
	Super::BeginPlay();	
}

void ACUiActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACUiActor::BindMuseumDelegate()
{
	if (!OwnerPlayer || !RelicCollectionWidget) return;
	UCMuseumComponent* MuseumComponent = OwnerPlayer->GetComponentByClass<UCMuseumComponent>();
	if (!MuseumComponent) return;

	MuseumComponent->OnRelicPlace.BindUObject(RelicCollectionWidget, &UCRelicCollectionWidget::InitRelicWidgets);
}

void ACUiActor::BindUiAnimDelegateForMuseum()
{
	if (!OwnerPlayer) return;
	
	UCMuseumComponent* MuseumComponent = OwnerPlayer->GetComponentByClass<UCMuseumComponent>();
	if (!MuseumComponent) return;

	MuseumComponent->OnUiAnimPlay.BindUObject(MuseumWidget, &UCWidgetBase::K2_PlayUiAnim);
}
