// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/Excavation/BrushTool.h"
#include "LHM/Excavation/RelicsBase.h"
#include "Components/BoxComponent.h"
#include "Components/DecalComponent.h"

// Sets default values
ABrushTool::ABrushTool()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	BrushMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BrushMesh"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Game/LHM/Meshes/SM_Brush.SM_Brush"));
	if (MeshAsset.Succeeded())
	{
		BrushMesh->SetStaticMesh(MeshAsset.Object);
		BrushMesh->SetupAttachment(RootComponent);
		BrushMesh->SetRelativeLocation(FVector(10, 0, 0)); // (X=10.000000,Y=-0.000000,Z=-0.000000)
		BrushMesh->SetRelativeRotation(FRotator(90, 0, 180)); // (Pitch=90.000000,Yaw=0.000000,Roll=180.000000)
	}

	BoxMesh = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxMesh"));
	BoxMesh->SetupAttachment(BrushMesh);
	BoxMesh->SetBoxExtent(FVector(8, 3, 5)); // (X=8.000000,Y=3.000000,Z=5.000000)
	BoxMesh->SetRelativeLocation(FVector(0, 0, 18)); // (X=0.000000,Y=0.000000,Z=18.000000)
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

	SwipeSpeed = FVector::Dist(BrushMesh->GetComponentLocation(), PreviousLocation) / DeltaTime;
	PreviousLocation = BrushMesh->GetComponentLocation();

	if (bIsBrushing) CheckBrushSwipe(DeltaTime);
}

void ABrushTool::OnBeginOverlap(UPrimitiveComponent* Overlapped, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 BodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	AActor* Target = OtherActor;

	if (!Target && OtherComp)
	{
		Target = OtherComp->GetOwner();
	}

	if (ARelicsBase* Relic = Cast<ARelicsBase>(Target))
	{
		CurrentOverlappingRelic = Relic;
		UE_LOG(LogTemp, Log, TEXT("[BrushTool] Overlapped with Relic: %s"), *Relic->GetName());
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

	if (SwipeSpeed > BrushSwipeThreshold)
	{
		ARelicsBase* Relic = Cast<ARelicsBase>(CurrentOverlappingRelic);
		if (Relic)
		{
			Relic->ReduceDustOpacity(BrushMesh->GetComponentLocation(), FadeSpeed * DeltaTime);
		}
	}
}

