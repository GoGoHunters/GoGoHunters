// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/Excavation/CollectionBox.h"
#include "Components/BoxComponent.h"
#include "LHM/Excavation/RelicsBase.h"
#include "Kismet/GameplayStatics.h"
#include "LHM/Excavation/ExcavationManager.h"
#include "EngineUtils.h"
#include "LHM/UI/BrushingUI.h"
#include "LHM/UI/DecalProgressUI.h"

// Sets default values
ACollectionBox::ACollectionBox()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));

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
	TriggerVolume->OnComponentEndOverlap.AddDynamic(this, &ACollectionBox::OnOverlapEnd);
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
			Mesh->ComponentTags.Add(FName("Collected"));

			// 이펙트/사운드/텍스트
			UGameplayStatics::PlaySound2D(GetWorld(), CollectionSFX); // 수거 효과음
			CollectedMeshes.Add(Mesh);

			UE_LOG(LogTemp, Log, TEXT("[CollectionBox] 유물 수거됨: %s"), *Relic->GetName());

			// UI 업데이트
			/*if (Relic->GetBrushingUI() && Relic->GetBrushingUI()->GetMeshToWidgetMap().Contains(Mesh))
			{
				if (UDecalProgressUI* Widget = Relic->GetBrushingUI()->GetMeshToWidgetMap()[Mesh])
				{
					Widget->SetCollectedImage(true);
				}
			}*/
			if (Relic->GetBrushingUI())
			{
				Relic->GetBrushingUI()->SetCollectedImage(true);
			}

			CheckAllCollected();
			break;
		}
	}
}

void ACollectionBox::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ARelicsBase* Relic = Cast<ARelicsBase>(OtherActor);
	if (!Relic) return;

	for (UStaticMeshComponent* Mesh : Relic->RelicsMeshes)
	{
		if (OtherComp == Mesh)
		{
			// 수거 목록에서 제거
			CollectedMeshes.Remove(Mesh);

			// UI 되돌리기
			/*if (Relic->GetBrushingUI() && Relic->GetBrushingUI()->GetMeshToWidgetMap().Contains(Mesh))
			{
				if (UDecalProgressUI* Widget = Relic->GetBrushingUI()->GetMeshToWidgetMap()[Mesh])
				{
					Widget->SetCollectedImage(false);
				}
			}*/
			if (Relic->GetBrushingUI())
			{
				Relic->GetBrushingUI()->SetCollectedImage(false);
			}

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
		//PlayBoxCloseAnimation();

		for (TActorIterator<AExcavationManager> It(GetWorld()); It; ++It)
		{
			It->NotifyCollectionCompleted(RelicsManager, this);
			break;
		}
	}
}

void ACollectionBox::PlayBoxCloseAnimation()
{
	K2_CloseLid();
}

