#include "UIs/CUiActor.h"

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
