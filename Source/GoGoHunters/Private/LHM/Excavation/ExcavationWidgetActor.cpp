// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/Excavation/ExcavationWidgetActor.h"
#include "Components/WidgetComponent.h"
#include "LHM/UI/ExcavationPhaseUI.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AExcavationWidgetActor::AExcavationWidgetActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));

	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClassFinder(TEXT("/Game/LHM/UI/WBP_ExcavationPhaseUI"));
	if (WidgetClassFinder.Succeeded())
	{
		WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("PhaseUI"));
		WidgetComponent->SetWidgetClass(WidgetClassFinder.Class);
		WidgetComponent->SetupAttachment(RootComponent);
		WidgetComponent->SetWidgetSpace(EWidgetSpace::World);
		WidgetComponent->SetRelativeScale3D(FVector(0.1));
		WidgetComponent->SetRelativeRotation(FRotator(0, 180, 0)); // (Pitch=0.000000,Yaw=180.000000,Roll=0.000000)
		WidgetComponent->SetDrawSize(FVector2D(1920, 1080));

		WidgetComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		WidgetComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		WidgetComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
		WidgetComponent->SetCollisionProfileName("VRUI");
	}
}

// Called when the game starts or when spawned
void AExcavationWidgetActor::BeginPlay()
{
	Super::BeginPlay();
	
	if (WidgetComponent)
	{
		UUserWidget* UserWidget = WidgetComponent->GetWidget();
		if (UExcavationPhaseUI* PhaseUI = Cast<UExcavationPhaseUI>(UserWidget))
		{
			PhaseUI->SetOwningWidgetActor(this);
		}
	}
}

// Called every frame
void AExcavationWidgetActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (WidgetComponent)
	{
		APlayerCameraManager* CamManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
		if (CamManager)
		{
			FVector CamLocation = CamManager->GetCameraLocation();
			FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(WidgetComponent->GetComponentLocation(), CamLocation);
			WidgetComponent->SetWorldRotation(LookAtRotation);
		}
	}
}

