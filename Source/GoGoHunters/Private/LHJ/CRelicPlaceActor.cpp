#include "LHJ/CRelicPlaceActor.h"
#include "Utilities/CHelpers.h"
#include "Components/BoxComponent.h"
#include "JMH/CMuseumComponent.h"
#include "LHJ/CRelicBase.h"

ACRelicPlaceActor::ACRelicPlaceActor()
{
	PrimaryActorTick.bCanEverTick = true;
	CHelpers::CreateComponent<USceneComponent>(this, &RootScene, "RootScene");
	
	CHelpers::CreateComponent<UBoxComponent>(this, &DetectCollision, "DetectCollision", RootComponent);
	DetectCollision->SetCollisionProfileName(FName("PlaceActor"));
	
	CHelpers::CreateComponent<UStaticMeshComponent>(this, &PlaceMesh, "PlaceMesh", RootComponent);
	PlaceMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PlaceMesh->SetCastShadow(false);

	// TODO 위젯 추가
}

void ACRelicPlaceActor::RerunConstructionScripts()
{
	Super::RerunConstructionScripts();
	DetectCollision->SetRelativeLocationAndRotation(DetectCollisionLocation, DetectCollisionRotation);
	DetectCollision->SetRelativeScale3D(DetectCollisionSize);
	PlaceMesh->SetRelativeLocationAndRotation(PlaceMeshLocation, PlaceMeshRotation);
	PlaceMesh->SetRelativeScale3D(PlaceMeshSize);
}

void ACRelicPlaceActor::BeginPlay()
{
	Super::BeginPlay();
	MuseumComp = GetWorld()->GetFirstPlayerController()->GetPawn()->GetComponentByClass<UCMuseumComponent>();

	// 다이나믹 머티리얼 만들어서 색 추가
	UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(PlaceMesh->GetMaterial(0), PlaceMesh);
	PlaceMesh->SetMaterial(0, DynamicMaterial);
}

void ACRelicPlaceActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateGridMeshComponents();
}

void ACRelicPlaceActor::UpdateGridMeshComponents() const
{
	if (MuseumComp && MuseumComp->GetMuseumState() == Decorate)
	{
		PlaceMesh->SetVisibility(true);

		FColor Color = bRegisterRelic ? FColor::Red : FColor::Green;
		UMaterialInstanceDynamic* DynamicMaterial = Cast<UMaterialInstanceDynamic>(PlaceMesh->GetMaterial(0));
		if (DynamicMaterial)
			DynamicMaterial->SetVectorParameterValue(TEXT("Color"), FLinearColor(Color));
	}
	else
	{
		PlaceMesh->SetVisibility(false);
	}
}

void ACRelicPlaceActor::RegisterRelic(const ACRelicBase* InRegisterRelic)
{
	const FVector PlaceRelicLocation = InRegisterRelic->GetActorLocation();
	FCRelicData PlaceRelicData = InRegisterRelic->GetRelicData();
	FCRelicDetailData PlaceRelicDetailData = InRegisterRelic->GetRelicDetailData();
	bRegisterRelic = true;

	// TODO 위젯 업데이트
	// UpdateDescriptionWidget(i, true, PlaceRelicData, PlaceRelicDetailData);
}

void ACRelicPlaceActor::UnRegisterRelic(const ACRelicBase* InUnRegisterRelic)
{
	if (!InUnRegisterRelic) return;
	bRegisterRelic = false;

	// TODO 위젯 업데이트
	// UpdateDescriptionWidget(i, false);
}

void ACRelicPlaceActor::SetPlaceRelicAtLocation(ACRelicBase* Relic, int32 PlaceIdx)
{
	if (!Relic) return;
	FCRelicData PlaceRelicData = Relic->GetRelicData();
	FCRelicDetailData PlaceRelicDetailData = Relic->GetRelicDetailData();
	
	Relic->SetActorLocation(GetActorLocation());
	Relic->SetActorScale3D(PlaceRelicScale);
	Relic->SetActorRotation(FRotator::ZeroRotator);

	// TODO 위젯 업데이트
	// UpdateDescriptionWidget(PlaceIdx, true, PlaceRelicData, PlaceRelicDetailData);
}
