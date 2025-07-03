#include "LHJ/CWorldMap.h"
#include "Utilities/CHelpers.h"

ACWorldMap::ACWorldMap()
{
	PrimaryActorTick.bCanEverTick = true;

	CHelpers::CreateComponent<USceneComponent>(this, &RootScene, "RootScene");
	CHelpers::CreateComponent<UStaticMeshComponent>(this, &Korea, "Korea", RootComponent);
	CHelpers::CreateComponent<UStaticMeshComponent>(this, &NorthAmerica, "NorthAmerica", RootComponent);
	CHelpers::CreateComponent<UStaticMeshComponent>(this, &SouthAmerica, "SouthAmerica", RootComponent);
	CHelpers::CreateComponent<UStaticMeshComponent>(this, &Asia, "Asia", RootComponent);
	CHelpers::CreateComponent<UStaticMeshComponent>(this, &Oceania, "Oceania", RootComponent);
	CHelpers::CreateComponent<UStaticMeshComponent>(this, &Europe, "Europe", RootComponent);
	CHelpers::CreateComponent<UStaticMeshComponent>(this, &Africa, "Africa", RootComponent);

	SetComonentInit(Korea);
	SetComonentInit(NorthAmerica);
	SetComonentInit(SouthAmerica);
	SetComonentInit(Asia);
	SetComonentInit(Oceania);
	SetComonentInit(Europe);
	SetComonentInit(Africa);
}

void ACWorldMap::BeginPlay()
{
	Super::BeginPlay();
}

void ACWorldMap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACWorldMap::SetComonentInit(UStaticMeshComponent* Comp)
{
	Comp->SetCollisionProfileName(FName("Continent"));
	Comp->SetRelativeScale3D(FVector(0.045));
	Comp->SetRelativeLocation(FVector(0, 0, 1));
	Comp->SetCastShadow(false);
}

void ACWorldMap::EnableCompOutline(UStaticMeshComponent* Comp, bool bEnable)
{
	if (!Comp) return;

	// 동적 머티리얼 인스턴스가 이미 할당되어 있는지 확인
	UMaterialInstanceDynamic* DynMat = Cast<UMaterialInstanceDynamic>(Comp->GetMaterial(0));
	if (!DynMat)
	{
		DynMat = Comp->CreateAndSetMaterialInstanceDynamic(0);
	}

	if (DynMat)
	{
		if (bEnable)
		{
			// 선택 시 색상 (예: 빨간색)
			DynMat->SetVectorParameterValue("Color", FLinearColor::Red);
		}
		else
		{
			// 해제 시 색상 (예: 흰색)
			DynMat->SetVectorParameterValue("Color", FLinearColor::White);
		}
	}
}
