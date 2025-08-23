#include "LHJ/CRelicDescActor.h"
#include "Components/WidgetComponent.h"
#include "Utilities/CHelpers.h"

ACRelicDescActor::ACRelicDescActor()
{
	PrimaryActorTick.bCanEverTick = true;
	CHelpers::CreateComponent<UStaticMeshComponent>(this, &MeshComp, "MeshComp");
	MeshComp->SetCastShadow(false);
	
	CHelpers::CreateComponent<UWidgetComponent>(this, &DescWidget, "DescWidget", RootComponent);
	DescWidget->SetCastShadow(false);
}

void ACRelicDescActor::BeginPlay()
{
	Super::BeginPlay();
}

void ACRelicDescActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACRelicDescActor::UpdateDescriptionWidget(bool bUpdate, FCRelicData InRelicData,
											   FCRelicDetailData InRelicDetailData)
{
	UUserWidget* WBPDescriptionWidget = DescWidget->GetWidget();
	if (!WBPDescriptionWidget) return;

	FName FunctionName(TEXT("UpdateData"));
	UFunction* Function = WBPDescriptionWidget->FindFunction(FunctionName);
	if (Function)
	{
		FCRelicDataParam param;
		param.RelicData = InRelicData;
		param.RelicDetailData = InRelicDetailData;
		param.IsUpdate = bUpdate;
		WBPDescriptionWidget->ProcessEvent(Function, &param);
	}
}
