// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/Excavation/BrushTool.h"
#include "LHM/Excavation/RelicsBase.h"
#include "Components/BoxComponent.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABrushTool::ABrushTool()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	BrushMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BrushMesh"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(TEXT("/Game/JMH/Anim/Brush/Brush_Anim01.Brush_Anim01"));
	if (MeshAsset.Succeeded())
	{
		BrushMesh->SetSkeletalMesh(MeshAsset.Object);
		BrushMesh->SetupAttachment(RootComponent);
		BrushMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		BrushMesh->bReceivesDecals = false;
	}

	BoxMesh = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxMesh"));
	BoxMesh->SetupAttachment(BrushMesh);
	BoxMesh->SetBoxExtent(FVector(16, 25, 12)); // (X=16.000000,Y=25.000000,Z=12.000000)
	BoxMesh->SetRelativeLocation(FVector(70, 0, 8)); // (X=70.000000,Y=-0.000000,Z=8.000000)
	BoxMesh->SetGenerateOverlapEvents(true);
	BoxMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxMesh->SetCollisionObjectType(ECC_WorldDynamic);
	BoxMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	BoxMesh->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);

	BoxMesh->OnComponentBeginOverlap.AddDynamic(this, &ABrushTool::OnBeginOverlap);
	BoxMesh->OnComponentEndOverlap.AddDynamic(this, &ABrushTool::OnEndOverlap);
}

// Called when the game starts or when spawned
void ABrushTool::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ABrushTool::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SwipeSpeed = FVector::Dist(BoxMesh->GetComponentLocation(), PreviousLocation) / DeltaTime;
	PreviousLocation = BoxMesh->GetComponentLocation();

	if (bIsBrushing) CheckBrushSwipe(DeltaTime);
}

void ABrushTool::OnBeginOverlap(UPrimitiveComponent* Overlapped, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 BodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	if (ARelicsBase* Relic = Cast<ARelicsBase>(OtherActor))
	{
		CurrentOverlappingRelic = Relic;
	}
}

void ABrushTool::OnEndOverlap(UPrimitiveComponent* Overlapped, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 BodyIndex)
{
	if (OtherActor == CurrentOverlappingRelic)
	{
		CurrentOverlappingRelic = nullptr;
	}
}

void ABrushTool::CheckBrushSwipe(float DeltaTime)
{
	if (!CurrentOverlappingRelic) return;

	if (SwipeSpeed > BrushSwipeThresholdMin
		&& SwipeSpeed < BrushSwipeThresholdMax)
	{
		ARelicsBase* Relic = Cast<ARelicsBase>(CurrentOverlappingRelic);
		if (Relic)
		{
			Relic->ReduceDustOpacity(BoxMesh->GetComponentLocation(), FadeSpeed * DeltaTime, *this);
		}
	}
}

void ABrushTool::UpdateFeedback(float Intensity)
{
	PlayVibration(Intensity);
	UpdateVisualFeedback(Intensity);
	PlaySoundFeedback(Intensity);
}

void ABrushTool::StopFeedback()
{
	bIsBrushing = false;

	// 햅틱 중지
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC)
	{
		PC->StopHapticEffect(EControllerHand::Right);
	}
}

void ABrushTool::PlayVibration(float Intensity)
{
	// 햅틱 피드백 재생
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC && HapticEffect)
	{
		float ClampedIntensity = FMath::Clamp(Intensity, 0.0f, 1.0f);
		PC->PlayHapticEffect(HapticEffect, EControllerHand::Right, ClampedIntensity, false);
	}
}

void ABrushTool::UpdateVisualFeedback(float Intensity)
{

}

void ABrushTool::PlaySoundFeedback(float Intensity)
{

}

void ABrushTool::SetIsBrushing(bool _bIsBrushing)
{
	bIsBrushing = _bIsBrushing;

	if (!bIsBrushing) StopFeedback();
}

