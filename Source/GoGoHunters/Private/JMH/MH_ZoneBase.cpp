// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_ZoneBase.h"

// Sets default values
AMH_ZoneBase::AMH_ZoneBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ZoneVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ZoneVisual"));
	RootComponent = ZoneVisual;

	ZoneVisual->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ZoneVisual->SetCollisionObjectType(ECC_WorldDynamic);
	ZoneVisual->SetCollisionResponseToAllChannels(ECR_Ignore);
	ZoneVisual->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block); 

}

// Called when the game starts or when spawned
void AMH_ZoneBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMH_ZoneBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMH_ZoneBase::OnPlayerInteracted_Implementation(AMH_ZoneBase* Player)
{
	if (!GuideMessage.IsEmpty())
	{
		GEngine->AddOnScreenDebugMessage(-1,2.f,FColor::Red,GuideMessage);
	}
}

