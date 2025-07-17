// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/Excavation/TweezersTool.h"
#include "Components/BoxComponent.h"
#include "LHM/Excavation/RelicsBase.h"

// Sets default values
ATweezersTool::ATweezersTool()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));

	TweezersMeshL = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TweezersMeshL"));
	TweezersMeshR = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TweezersMeshR"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAssets(TEXT("/Game/LHM/Meshes/SM_Tweezerses.SM_Tweezerses"));
	if (MeshAssets.Succeeded())
	{
		TweezersMeshL->SetStaticMesh(MeshAssets.Object);
		TweezersMeshL->SetupAttachment(RootComponent);
		TweezersMeshL->SetRelativeLocation(FVector(25, -2, -70)); // (X=25.000000,Y=-2.000000,Z=10.000000)
		TweezersMeshL->SetRelativeRotation(FRotator(0, -5, 0)); // (Pitch=0.000000,Yaw=-5.000000,Roll=-0.000000)

		TweezersMeshR->SetStaticMesh(MeshAssets.Object);
		TweezersMeshR->SetupAttachment(RootComponent);
		TweezersMeshR->SetRelativeLocation(FVector(25, 2, -70)); // (X=25.000000,Y=2.000000,Z=10.000000)
		TweezersMeshR->SetRelativeRotation(FRotator(0, 5, 0)); // (Pitch=0.000000,Yaw=5.000000,Roll=-0.000000)
	}

	PickupBox = CreateDefaultSubobject<UBoxComponent>(TEXT("PickupBox"));
	PickupBox->SetupAttachment(RootComponent);

	PickupPoint = CreateDefaultSubobject<USceneComponent>(TEXT("PickupPoint"));
	PickupPoint->SetupAttachment(RootComponent);
	//PickupPoint->SetRelativeLocation(FVector(30.f, 0.f, 10.f));
}

// Called when the game starts or when spawned
void ATweezersTool::BeginPlay()
{
	Super::BeginPlay();
	
	PickupBox->OnComponentBeginOverlap.AddDynamic(this, &ATweezersTool::OnBeginOverlap);
}

// Called every frame
void ATweezersTool::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(bIsPickingUp) PickUpRelic();
}

void ATweezersTool::PickUpRelic()
{
	if (!bIsPickingUp || PickedRelic || !RelicCandidate || !CandidateMesh) return;

	// Attach 조건 만족 → 실행
	CandidateMesh->SetSimulatePhysics(false);
	CandidateMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CandidateMesh->AttachToComponent(PickupPoint, FAttachmentTransformRules::SnapToTargetIncludingScale);

	PickedRelic = RelicCandidate;

	UE_LOG(LogTemp, Log, TEXT("[TweezersTool] Picked up %s"), *CandidateMesh->GetName());

	// 초기화
	RelicCandidate = nullptr;
	CandidateMesh = nullptr;
}

void ATweezersTool::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (PickedRelic || !OtherActor || !OtherComp) return;

	if (ARelicsBase* Relic = Cast<ARelicsBase>(OtherActor))
	{
		for (UStaticMeshComponent* Mesh : Relic->RelicsMeshes)
		{
			if (OtherComp == Mesh)
			{
				RelicCandidate = Relic;
				CandidateMesh = Mesh;
				break;
			}
		}
	}
}

void ATweezersTool::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == RelicCandidate || OtherComp == CandidateMesh)
	{
		RelicCandidate = nullptr;
		CandidateMesh = nullptr;
	}
}

void ATweezersTool::SetIsPickingUp(bool _bIsPickingUp)
{
	bIsPickingUp = _bIsPickingUp;

	UE_LOG(LogTemp, Log, TEXT("[TweezersTool] SetIsPickingUp: %s"), bIsPickingUp ? TEXT("True") : TEXT("False"));

	if (bIsPickingUp)
	{
		TweezersMeshL->SetRelativeLocation(FVector(25, -1, -70));
		TweezersMeshL->SetRelativeRotation(FRotator(0, 0, 0));
		TweezersMeshR->SetRelativeLocation(FVector(25, 1, -70));
		TweezersMeshR->SetRelativeRotation(FRotator(0, 0, 0));
	}
	else
	{
		TweezersMeshL->SetRelativeLocation(FVector(25, -2, -70));
		TweezersMeshL->SetRelativeRotation(FRotator(0, -5, 0));
		TweezersMeshR->SetRelativeLocation(FVector(25, 2, -70));
		TweezersMeshR->SetRelativeRotation(FRotator(0, 5, 0));

		// 유물 놓기
		if (PickedRelic)
		{
			for (UStaticMeshComponent* Mesh : PickedRelic->RelicsMeshes)
			{
				if (Mesh->GetAttachParent() == PickupPoint)
				{
					Mesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
					Mesh->SetSimulatePhysics(true);
					Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
					UE_LOG(LogTemp, Log, TEXT("[TweezersTool] Dropped relic mesh"));
					break;
				}
			}

			PickedRelic = nullptr;
		}
	}

}

