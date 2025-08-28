// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/Excavation/ExcavationWarningWidgetActor.h"
#include "Components/WidgetComponent.h"
#include "LHM/UI/WarningUI.h"

// Sets default values
AExcavationWarningWidgetActor::AExcavationWarningWidgetActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));

	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClassFinder(TEXT("/Game/LHM/UI/WBP_WarningUI"));
	if (WidgetClassFinder.Succeeded())
	{
		WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WarningUI"));
		WidgetComponent->SetWidgetClass(WidgetClassFinder.Class);
		WidgetComponent->SetupAttachment(RootComponent);
		WidgetComponent->SetWidgetSpace(EWidgetSpace::World);
		WidgetComponent->SetDrawSize(FVector2D(300,100));

		WidgetComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		WidgetComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		WidgetComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
		WidgetComponent->SetCollisionProfileName("VRUI");

		WidgetComponent->SetHiddenInGame(true);
	}
}

// Called when the game starts or when spawned
void AExcavationWarningWidgetActor::BeginPlay()
{
	Super::BeginPlay();

	if (WidgetComponent)
	{
		UUserWidget* UserWidget = WidgetComponent->GetWidget();
		if (UWarningUI* WarningUI = Cast<UWarningUI>(UserWidget))
		{
			WarningUI->SetOwningWidgetActor(this);
		}
	}
}

// Called every frame
void AExcavationWarningWidgetActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

