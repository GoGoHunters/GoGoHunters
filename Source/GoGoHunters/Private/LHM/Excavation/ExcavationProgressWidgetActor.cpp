// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/Excavation/ExcavationProgressWidgetActor.h"
#include "Components/WidgetComponent.h"
#include "LHM/UI/ExcavationProgressUI.h"

// Sets default values
AExcavationProgressWidgetActor::AExcavationProgressWidgetActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));

	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClassFinder(TEXT("/Game/LHM/UI/WBP_ExcavationProgressUI"));
	if (WidgetClassFinder.Succeeded())
	{
		WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("ProgressUI"));
		WidgetComponent->SetWidgetClass(WidgetClassFinder.Class);
		WidgetComponent->SetupAttachment(RootComponent);
		WidgetComponent->SetWidgetSpace(EWidgetSpace::World);
		//WidgetComponent->SetRelativeScale3D(FVector(0.3));
		//WidgetComponent->SetRelativeRotation(FRotator(0, 90, 0));
		//WidgetComponent->SetDrawSize(FVector2D(300, 100));

		WidgetComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		WidgetComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		WidgetComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
		WidgetComponent->SetCollisionProfileName("VRUI");
	}
}

// Called when the game starts or when spawned
void AExcavationProgressWidgetActor::BeginPlay()
{
	Super::BeginPlay();

	if (WidgetComponent)
	{
		UUserWidget* UserWidget = WidgetComponent->GetWidget();
		if (UExcavationProgressUI* ProgressUI = Cast<UExcavationProgressUI>(UserWidget))
		{
			ProgressUI->SetOwningWidgetActor(this);
		}
	}
}

// Called every frame
void AExcavationProgressWidgetActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

