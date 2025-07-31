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
	static ConstructorHelpers::FObjectFinder<UStaticMesh> LMeshAsset(TEXT("/Game/JMH/Mesh/04_Assets/Tools/Tools02_1.Tools02_1"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> RMeshAsset(TEXT("/Game/JMH/Mesh/04_Assets/Tools/Tools02_2.Tools02_2"));
	if (LMeshAsset.Succeeded() && RMeshAsset.Succeeded())
	{
		TweezersMeshL->SetStaticMesh(LMeshAsset.Object);
		TweezersMeshL->SetupAttachment(RootComponent);
		TweezersMeshL->SetRelativeLocation(FVector(20, 0, 0));
		TweezersMeshL->SetRelativeRotation(FRotator(0, -90, 0));
		TweezersMeshL->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		TweezersMeshL->bReceivesDecals = false;

		TweezersMeshR->SetStaticMesh(RMeshAsset.Object);
		TweezersMeshR->SetupAttachment(RootComponent);
		TweezersMeshR->SetRelativeLocation(FVector(20, 0, 0));
		TweezersMeshR->SetRelativeRotation(FRotator(0, -90, 0));
		TweezersMeshR->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		TweezersMeshL->bReceivesDecals = false;
	}

	PickupBox = CreateDefaultSubobject<UBoxComponent>(TEXT("PickupBox"));
	PickupBox->SetupAttachment(RootComponent);

	PickupPoint = CreateDefaultSubobject<USceneComponent>(TEXT("PickupPoint"));
	PickupPoint->SetupAttachment(RootComponent);
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
	if (/*PickedRelic ||*/ !OtherActor || !OtherComp) return;

	if (ARelicsBase* Relic = Cast<ARelicsBase>(OtherActor))
	{
		for (UStaticMeshComponent* Mesh : Relic->RelicsMeshes)
		{
			if (OtherComp == Mesh)
			{
				if (Mesh->ComponentTags.Contains("Collected")) return; // 이미 수거된 유물은 무시
			
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

	if (bIsPickingUp)
	{
		TweezersMeshL->SetRelativeLocation(FVector(20, 0, 0));
		TweezersMeshL->SetRelativeRotation(FRotator(0, -82, 0));
		TweezersMeshR->SetRelativeLocation(FVector(20, 0, 0));
		TweezersMeshR->SetRelativeRotation(FRotator(0, -97, 0));
	}
	else
	{
		TweezersMeshL->SetRelativeLocation(FVector(20, 0, 0));
		TweezersMeshL->SetRelativeRotation(FRotator(0, -90, 0));
		TweezersMeshR->SetRelativeLocation(FVector(20, 0, 0));
		TweezersMeshR->SetRelativeRotation(FRotator(0, -90, 0));

		// 유물 놓기
		if (PickedRelic)
		{
			for (UStaticMeshComponent* Mesh : PickedRelic->RelicsMeshes)
			{
				if (Mesh->GetAttachParent() == PickupPoint)
				{
					Mesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
					Mesh->SetSimulatePhysics(true);
					Mesh->SetCollisionProfileName(FName("Relic_Physics"));
					Mesh->SetGenerateOverlapEvents(true);
					Mesh->BodyInstance.bUseCCD = true; // 빠르게 낙하 시 충돌 누락 방지

					UE_LOG(LogTemp, Log, TEXT("[TweezersTool] Dropped relic mesh"));
					break;
				}
			}

			PickedRelic = nullptr;
		}
	}

}

