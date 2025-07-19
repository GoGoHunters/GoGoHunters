// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/Excavation/CollectionBox.h"
#include "Components/BoxComponent.h"
#include "LHM/Excavation/RelicsBase.h"
#include "Kismet/GameplayStatics.h"
#include "LHM/Excavation/ExcavationManager.h"
#include "EngineUtils.h"

// Sets default values
ACollectionBox::ACollectionBox()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	CollectionBox = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CollectionBox"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Game/LHM/Meshes/SM_CollectionBox.SM_CollectionBox"));
	if (MeshAsset.Succeeded())
	{
		CollectionBox->SetStaticMesh(MeshAsset.Object);
		CollectionBox->SetupAttachment(RootComponent);
		CollectionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		CollectionBox->SetCollisionObjectType(ECC_WorldStatic);
		CollectionBox->SetCollisionResponseToAllChannels(ECR_Block);
	}

	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
	TriggerVolume->SetupAttachment(RootComponent);
	TriggerVolume->SetBoxExtent(FVector(20));
	TriggerVolume->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	TriggerVolume->SetGenerateOverlapEvents(true);

}

// Called when the game starts or when spawned
void ACollectionBox::BeginPlay()
{
	Super::BeginPlay();

	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &ACollectionBox::OnOverlapBegin);

}

// Called every frame
void ACollectionBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACollectionBox::OnOverlapBegin(UPrimitiveComponent* Overlapped, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	ARelicsBase* Relic = Cast<ARelicsBase>(OtherActor);
	if (!Relic) return;

	for (UStaticMeshComponent* Mesh : Relic->RelicsMeshes)
	{
		if (OtherComp == Mesh)
		{
			// 수거 처리: Detach + 물리/충돌 제거
			Mesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
			/*FTimerHandle TimerHandle;
			GetWorldTimerManager().SetTimer(TimerHandle, [Mesh]()
			{
				Mesh->SetSimulatePhysics(false);
				Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}, 0.01f, false);*/

			// 이펙트/사운드/텍스트
			UGameplayStatics::PlaySound2D(GetWorld(), CollectionSFX); // 수거 효과음
			UE_LOG(LogTemp, Log, TEXT("[CollectionBox] 유물 수거됨: %s"), *Relic->GetName());

			CollectedMeshes.Add(Mesh);
			CheckAllCollected();
			break;
		}
	}
}

void ACollectionBox::CheckAllCollected()
{
	if(!TargetRelic) return;

	const int32 TotalCount = TargetRelic->RelicsMeshes.Num();
	const int32 CollectedCount = CollectedMeshes.Num();

	UE_LOG(LogTemp, Log, TEXT("[CollectionBox] Relics collected (%d/%d)"), CollectedCount, TotalCount);

	if (TotalCount > 0 && CollectedCount == TotalCount)
	{
		UE_LOG(LogTemp, Log, TEXT("[CollectionBox] 모든 유물 총 %d개 수거 완료!"), CollectedCount);
		PlayBoxCloseAnimation();

		for (TActorIterator<AExcavationManager> It(GetWorld()); It; ++It)
		{
			It->NotifyCollectionCompleted(RelicsManager);
			break;
		}
	}
}

void ACollectionBox::PlayBoxCloseAnimation()
{
	// 포장 애니메이션 재생
}

