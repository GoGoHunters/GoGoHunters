// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/Restore/RestorWidgetActor.h"
#include "Components/WidgetComponent.h"
#include "LHM/UI/RestoreUI.h"
#include "LHM/UI/RestorationCompleteUI.h"
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

    RestoreWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("RestoreUI"));
    RestoreWidgetComp->SetupAttachment(BookMesh);
    static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClassFinder(TEXT("/Game/LHM/UI/WBP_RestoreUI"));
    if (WidgetClassFinder.Succeeded())
    {
        RestoreWidgetComp->SetWidgetClass(WidgetClassFinder.Class);
    }

	CompleteWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("CompleteUI"));
	CompleteWidgetComp->SetupAttachment(BookMesh);
	static ConstructorHelpers::FClassFinder<UUserWidget> CompleteWidgetClassFinder(TEXT("/Game/LHM/UI/WBP_RestorationCompleteUI"));
    if (CompleteWidgetClassFinder.Succeeded())
    {
        CompleteWidgetComp->SetWidgetClass(CompleteWidgetClassFinder.Class);
        CompleteWidgetComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		CompleteWidgetComp->SetCollisionProfileName("NoCollision");
		CompleteWidgetComp->SetHiddenInGame(true);
	}
}

void ARestorWidgetActor::ShowCompleteUI(bool bShow)
{
	if (!CompleteWidgetComp) return;
    if (bShow)
    {
        CompleteWidgetComp->SetHiddenInGame(false);
        CompleteWidgetComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        CompleteWidgetComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CompleteWidgetComp->SetCollisionProfileName("VRUI");
    }
	else
	{
		CompleteWidgetComp->SetHiddenInGame(true);
		CompleteWidgetComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        CompleteWidgetComp->SetCollisionProfileName("NoCollision");
	}
}

// Called when the game starts or when spawned
void ARestorWidgetActor::BeginPlay()
{
	Super::BeginPlay();

    if (UUserWidget* UserWidget = CompleteWidgetComp->GetWidget())
    {
        if(URestorationCompleteUI* CompleteUI = Cast<URestorationCompleteUI>(UserWidget))
        {
			CompleteUI->SetOwningWidgetActor(this);
		}
    }
}
