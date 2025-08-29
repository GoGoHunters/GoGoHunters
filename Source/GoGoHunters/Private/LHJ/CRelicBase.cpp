#include "LHJ/CRelicBase.h"

#include "LHJ/Pickup/CRelicPickupActorComponent.h"
#include "Utilities/CHelpers.h"

ACRelicBase::ACRelicBase()
{
	PrimaryActorTick.bCanEverTick = true;
	CHelpers::CreateComponent<UStaticMeshComponent>(this, &RelicMesh, "RelicMesh");
	RelicMesh->ComponentTags.Add("Pickup");
	RelicMesh->SetCollisionProfileName(FName("WorldDynamic"));
	RelicMesh->SetCollisionResponseToChannel(ECC_GameTraceChannel12, ECR_Block);

	CHelpers::CreateActorComponent<UCRelicPickupActorComponent>(this, &PickupActorComponent, "PickupActorComponent");
	PickupActorComponent->SetTwoHandGrabbing(true);
}

void ACRelicBase::BeginPlay()
{
	Super::BeginPlay();
	RelicMaterial = RelicMesh->GetMaterial(0);
}

void ACRelicBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACRelicBase::SetRelicMaterial(UMaterialInterface* NewRelicMaterial)
{
	if (!NewRelicMaterial) return;
	RelicMesh->SetMaterial(0, NewRelicMaterial);
}

void ACRelicBase::InitializeAsset(const FCRelicData& InRelicData, const FCRelicDetailData& InRelicDetailData)
{
	RelicData = InRelicData;
	RelicDetailData = InRelicDetailData;
}

void ACRelicBase::SetRelicMaterial()
{
	if(!RelicMaterial) return;
	RelicMesh->SetMaterial(0, RelicMaterial);
}

void ACRelicBase::ReturnToOriginalLocation()
{
	SetActorLocationAndRotation(RelicData.PlacedTransform.GetLocation(), RelicData.PlacedTransform.GetRotation());
	SetActorScale3D(RelicData.PlacedTransform.GetScale3D());
}

void ACRelicBase::SetRelicGrabScale()
{
	PickupActorComponent->SetGrabActorScale(GetActorScale3D());
}
