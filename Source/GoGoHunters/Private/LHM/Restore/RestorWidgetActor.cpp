// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/Restore/RestorWidgetActor.h"
#include "Components/WidgetComponent.h"
#include "LHM/UI/RestoreUI.h"
#include "base/GI_Base.h"

// Sets default values
ARestorWidgetActor::ARestorWidgetActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
    RootComponent = RootScene;

    BookMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BookMesh"));
    BookMesh->SetupAttachment(RootScene);

    WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
    WidgetComponent->SetupAttachment(BookMesh);
    static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClassFinder(TEXT("/Game/LHM/UI/WBP_RestoreUI"));
    if (WidgetClassFinder.Succeeded())
    {
        WidgetComponent->SetWidgetClass(WidgetClassFinder.Class);
    }
}

// Called when the game starts or when spawned
void ARestorWidgetActor::BeginPlay()
{
	Super::BeginPlay();
	
    
}
