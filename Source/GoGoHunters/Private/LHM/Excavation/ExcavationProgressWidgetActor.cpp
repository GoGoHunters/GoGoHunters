// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/Excavation/ExcavationProgressWidgetActor.h"
#include "Components/WidgetComponent.h"
#include "LHM/UI/ExcavationProgressUI.h"
#include "LHM/UI/WarningUI.h"

// Sets default values
AExcavationProgressWidgetActor::AExcavationProgressWidgetActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));

	static ConstructorHelpers::FClassFinder<UUserWidget> ProgressWidgetClassFinder(TEXT("/Game/LHM/UI/WBP_ExcavationProgressUI"));
	if (ProgressWidgetClassFinder.Succeeded())
	{
		ProgressWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("ProgressUI"));
		ProgressWidget->SetWidgetClass(ProgressWidgetClassFinder.Class);
		ProgressWidget->SetupAttachment(RootComponent);
		ProgressWidget->SetWidgetSpace(EWidgetSpace::World);

		ProgressWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		ProgressWidget->SetCollisionProfileName("NoCollision");
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> WarningWidgetClassFinder(TEXT("/Game/LHM/UI/WBP_WarningUI"));
	if (WarningWidgetClassFinder.Succeeded())
	{
		WarningWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("WarningUI"));
		WarningWidget->SetWidgetClass(WarningWidgetClassFinder.Class);
		WarningWidget->SetupAttachment(RootComponent);
		WarningWidget->SetWidgetSpace(EWidgetSpace::World);
		WarningWidget->SetDrawSize(FVector2D(300, 100));

		WarningWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		WarningWidget->SetCollisionProfileName("NoCollision");

		WarningWidget->SetHiddenInGame(true);
	}
}

// Called when the game starts or when spawned
void AExcavationProgressWidgetActor::BeginPlay()
{
	Super::BeginPlay();

	if (ProgressWidget)
	{
		UUserWidget* UserWidget = ProgressWidget->GetWidget();
		if (UExcavationProgressUI* ProgressUI = Cast<UExcavationProgressUI>(UserWidget))
		{
			ProgressUI->SetOwningWidgetActor(this);
		}
	}

	if (WarningWidget)
	{
		UUserWidget* UserWidget = WarningWidget->GetWidget();
		if (UWarningUI* WarningUI = Cast<UWarningUI>(UserWidget))
		{
			WarningUI->SetOwningWidgetActor(this);
		}
	}
}

// Called every frame
void AExcavationProgressWidgetActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

