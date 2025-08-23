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
	PlaceMesh->SetVisibility(false);
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
}

void ACRelicPlaceActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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
