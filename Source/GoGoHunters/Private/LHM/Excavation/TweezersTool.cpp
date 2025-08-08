// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/Excavation/TweezersTool.h"
#include "Components/BoxComponent.h"
#include "LHM/Excavation/RelicsBase.h"
#include "MotionControllerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"

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
		TweezersMeshL->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		TweezersMeshL->bReceivesDecals = false;

		TweezersMeshR->SetStaticMesh(RMeshAsset.Object);
		TweezersMeshR->SetupAttachment(RootComponent);
		TweezersMeshR->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		TweezersMeshR->bReceivesDecals = false;
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

	if (AttachBase && !bHasJustDropped)
	{
		FVector Current = AttachBase->GetComponentLocation();
		if (!Current.Equals(LastAttachLocation, 0.1f))
		{
			PreviousAttachLocation = LastAttachLocation;
			LastAttachLocation = Current;
		}
	}
	else if (bHasJustDropped)
	{
		bHasJustDropped = false;
	}
}

void ATweezersTool::PickUpRelic()
{
	if (!bIsPickingUp || PickedRelic || !RelicCandidate || !CandidateMesh) return;

	if (!PickupBox->IsOverlappingActor(RelicCandidate))
	{
		RelicCandidate = nullptr;
		CandidateMesh = nullptr;
		//UE_LOG(LogTemp, Warning, TEXT("[TweezersTool] Blocked pickup: not overlapping!"));
		return;
	}

	// Attach 조건 만족 → 실행
	CandidateMesh->SetSimulatePhysics(false);
	CandidateMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CandidateMesh->AttachToComponent(PickupPoint, FAttachmentTransformRules::SnapToTargetIncludingScale);

	// 사운드 재생
	if (SoundFX) UGameplayStatics::PlaySoundAtLocation(this, SoundFX, PickupPoint->GetComponentLocation());

	PickedRelic = RelicCandidate;

	// 타미 음성
	if(!bIsPlayingTami) PlayTami();

	//UE_LOG(LogTemp, Log, TEXT("[TweezersTool] Picked up %s"), *CandidateMesh->GetName());

	// 초기화
	RelicCandidate = nullptr;
	CandidateMesh = nullptr;
}

void ATweezersTool::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (/*PickedRelic ||*/ !OtherActor || !OtherComp) return;

	if (ARelicsBase* RelicsBase = Cast<ARelicsBase>(OtherActor))
	{
		for (UStaticMeshComponent* Relic : RelicsBase->RelicsMeshes)
		{
			if (OtherComp == Relic)
			{
				// 이미 수거된 유물은 무시
				if (Relic->ComponentTags.Contains("Collected"))
				{
					//UE_LOG(LogTemp, Warning, TEXT("[TweezersTool] Ignored already collected relic: %s"), *Relic->GetName());
					RelicCandidate = nullptr;
					CandidateMesh = nullptr;
					return;
				}
			
				RelicCandidate = RelicsBase;
				CandidateMesh = Relic;
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
		TweezersMeshL->SetRelativeRotation(FRotator(0, 7, 0));
		TweezersMeshR->SetRelativeRotation(FRotator(0, -7, 0));
	}
	else
	{
		TweezersMeshL->SetRelativeRotation(FRotator(0, 0, 0));
		TweezersMeshR->SetRelativeRotation(FRotator(0, 0, 0));

		// 유물 놓기
		if (PickedRelic)
		{
			for (UStaticMeshComponent* Relic : PickedRelic->RelicsMeshes)
			{
				if (Relic->GetAttachParent() == PickupPoint)
				{
					Relic->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
					Relic->SetSimulatePhysics(true);
					Relic->SetCollisionProfileName(FName("Relic_Physics"));
					Relic->SetGenerateOverlapEvents(true);
					Relic->BodyInstance.bUseCCD = true; // 빠르게 낙하 시 충돌 누락 방지

					// 던지기
					if (AttachBase)
					{
						FVector Velocity = (LastAttachLocation - PreviousAttachLocation) / FMath::Max(GetWorld()->GetDeltaSeconds(), 0.001f);
						FVector Direction = Velocity.GetSafeNormal();
						float Speed = Velocity.Size();
						//UE_LOG(LogTemp, Warning, TEXT("[TweezersTool] Velocity Raw: %s | SpeedRaw: %.2f"), *Velocity.ToString(), Speed);

						if (!Direction.IsNearlyZero())
						{
							Relic->SetPhysicsLinearVelocity(Direction * Speed);
						}
					}
					//UE_LOG(LogTemp, Log, TEXT("[TweezersTool] Dropped relic mesh"));
					break;
				}
			}
			PickedRelic = nullptr;
			bHasJustDropped = true;
		}
	}
}

void ATweezersTool::SetAttachBase(USceneComponent* InAttachBase)
{
	AttachBase = InAttachBase;
	if (AttachBase)
	{
		LastAttachLocation = AttachBase->GetComponentLocation();
		//UE_LOG(LogTemp, Warning, TEXT("[TweezersTool] AttachBase set to: %s"), *AttachBase->GetName());
	}
}

void ATweezersTool::PlayTami()
{
	bIsPlayingTami = true;

	APawn* TamiAI = nullptr;

	for (TActorIterator<APawn> It(GetWorld(), APawn::StaticClass()); It; ++It)
	{
		if (IsValid(*It) && (*It)->ActorHasTag(FName("Tami")))
		{
			TamiAI = *It;
			break;
		}
	}

	if (!TamiAI) return;

	FName FunctionName(TEXT("PlayExcavationPhase5_ExplainUsingTweezers2"));
	if (UFunction* Function = TamiAI->FindFunction(FunctionName))
	{
		TamiAI->ProcessEvent(Function, nullptr);
	}
}

