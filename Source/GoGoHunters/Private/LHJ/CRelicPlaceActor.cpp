#include "LHJ/CRelicPlaceActor.h"
#include "Utilities/CHelpers.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "JMH/CMuseumComponent.h"
#include "LHJ/CRelicBase.h"
#include "LHJ/CRelicDescActor.h"

ACRelicPlaceActor::ACRelicPlaceActor()
{
	PrimaryActorTick.bCanEverTick = true;
	CHelpers::CreateComponent<USceneComponent>(this, &RootScene, "RootScene");
	
	CHelpers::CreateComponent<UBoxComponent>(this, &DetectCollision, "DetectCollision", RootComponent);
	DetectCollision->SetCollisionProfileName(FName("PlaceActor"));
	
	CHelpers::CreateComponent<UStaticMeshComponent>(this, &PlaceMesh, "PlaceMesh", RootComponent);
	PlaceMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PlaceMesh->SetCastShadow(false);

	CHelpers::CreateComponent<UChildActorComponent>(this, &DescWidget, "DescWidget", RootComponent);
	
	CHelpers::CreateComponent<UWidgetComponent>(this, &RecoverRelicWidget, "RecoverRelicWidget", RootComponent);
	RecoverRelicWidget->SetCastShadow(false);
}

void ACRelicPlaceActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	DetectCollision->SetRelativeLocationAndRotation(DetectCollisionLocation, DetectCollisionRotation);
	DetectCollision->SetRelativeScale3D(DetectCollisionSize);
	
	PlaceMesh->SetRelativeLocationAndRotation(PlaceMeshLocation, PlaceMeshRotation);
	PlaceMesh->SetRelativeScale3D(PlaceMeshSize);
	
	DescWidget->SetRelativeLocationAndRotation(DescWidgetCompLocation, DescWidgetCompRotation);
	DescWidget->SetRelativeScale3D(DescWidgetCompSize);
}

void ACRelicPlaceActor::BeginPlay()
{
	Super::BeginPlay();
	Cast<ACRelicDescActor>(DescWidget->GetChildActor())->UpdateDescriptionWidget(false);
	MuseumComp = GetWorld()->GetFirstPlayerController()->GetPawn()->GetComponentByClass<UCMuseumComponent>();

	// 다이나믹 머티리얼 만들어서 색 추가
	UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(PlaceMesh->GetMaterial(0), PlaceMesh);
	PlaceMesh->SetMaterial(0, DynamicMaterial);

	FName FunctionName(TEXT("SetOwnerActor"));
	UFunction* Function = RecoverRelicWidget->GetWidget()->FindFunction(FunctionName);
	if (Function)
	{
		FCRelicPlaceActorParam Param;
		Param.RelicPlaceActor = this;
		RecoverRelicWidget->GetWidget()->ProcessEvent(Function, &Param);
	}
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

		FColor Color = bRegisterRelic ? FColor(79, 128, 255, 1.0) : FColor::Green;
		UMaterialInstanceDynamic* DynamicMaterial = Cast<UMaterialInstanceDynamic>(PlaceMesh->GetMaterial(0));
		if (DynamicMaterial)
			DynamicMaterial->SetVectorParameterValue(TEXT("Color"), FLinearColor(Color));

		if (bRegisterRelic)
			RecoverRelicWidget->SetVisibility(true);
		else
			RecoverRelicWidget->SetVisibility(false);
	}
	else
	{
		PlaceMesh->SetVisibility(false);
		RecoverRelicWidget->SetVisibility(false);
	}
}

void ACRelicPlaceActor::RegisterRelic(ACRelicBase* InRegisterRelic)
{
	const FVector PlaceRelicLocation = InRegisterRelic->GetActorLocation();
	FCRelicData PlaceRelicData = InRegisterRelic->GetRelicData();
	FCRelicDetailData PlaceRelicDetailData = InRegisterRelic->GetRelicDetailData();
	bRegisterRelic = true;
	RegisterRelicObj = InRegisterRelic;

	Cast<ACRelicDescActor>(DescWidget->GetChildActor())->UpdateDescriptionWidget(true, PlaceRelicData, PlaceRelicDetailData);
}

void ACRelicPlaceActor::UnRegisterRelic()
{
	bRegisterRelic = false;
	RegisterRelicObj = nullptr;

	Cast<ACRelicDescActor>(DescWidget->GetChildActor())->UpdateDescriptionWidget(false);
}

void ACRelicPlaceActor::SetPlaceRelicAtLocation(ACRelicBase* Relic)
{
	if (!Relic) return;
	FCRelicData PlaceRelicData = Relic->GetRelicData();
	FCRelicDetailData PlaceRelicDetailData = Relic->GetRelicDetailData();
	
	Relic->SetActorLocation(GetActorLocation());
	Relic->SetActorScale3D(PlaceRelicScale);
	Relic->SetActorRotation(FRotator::ZeroRotator);
	Relic->SetRelicGrabScale();

	Cast<ACRelicDescActor>(DescWidget->GetChildActor())->UpdateDescriptionWidget(true, PlaceRelicData, PlaceRelicDetailData);
}

void ACRelicPlaceActor::RecoverRelic()
{
	if (!RegisterRelicObj) return;
	
	FCRelicData data = RegisterRelicObj->GetRelicData();
	MuseumComp->RecoverRelic(data);
	RegisterRelicObj->Destroy();
	UnRegisterRelic();
}
