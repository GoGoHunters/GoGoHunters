// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/Excavation/RelicsBase.h"
#include "LHM/Excavation/ExcavationMarker.h"
#include "Components/DecalComponent.h"
#include "LHM/Excavation/ExcavationManager.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "LHM/Excavation/BrushTool.h"

// Sets default values
ARelicsBase::ARelicsBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));

    for (int i = 0; i < 5; ++i)
    {
        FString MeshName = FString::Printf(TEXT("RelicMesh_%d"), i + 1);
        UStaticMeshComponent* RelicMesh = CreateDefaultSubobject<UStaticMeshComponent>(*MeshName);
        RelicMesh->SetupAttachment(RootComponent);
        RelicMesh->SetCollisionProfileName(FName("Relic_Buried"));
        RelicMesh->SetGenerateOverlapEvents(true);
        RelicsMeshes.Add(RelicMesh);

        int DecalCount = (i == 0) ? 3 : 1;

        for (int j = 0; j < DecalCount; ++j)
        {
            FString DecalName = FString::Printf(TEXT("DustDecal_%d_%d"), i + 1, j + 1);
            UDecalComponent* Decal = CreateDefaultSubobject<UDecalComponent>(*DecalName);
            Decal->SetupAttachment(RelicMesh);
            DustDecals.Add(Decal);
            DecalToMeshMap.Add(Decal, RelicMesh);
        }
    }
}

void ARelicsBase::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    for (UDecalComponent* Decal : DustDecals)
    {
        UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(Decal->GetMaterial(0), this);
        Decal->SetMaterial(0, MID);
        MID->SetScalarParameterValue(OpacityParameterName, CurrentOpacity);
        DecalMIDs.Add(Decal, MID);
    }
}

// Called when the game starts or when spawned
void ARelicsBase::BeginPlay()
{
	Super::BeginPlay();

    if (MarkerClass)
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
        Marker = GetWorld()->SpawnActor<AExcavationMarker>(MarkerClass, GetActorLocation(), GetActorRotation(), SpawnParams);

        if (Marker)
        {
            Marker->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
            Marker->SetActorLocation(GetActorLocation()+FVector(0,0,280));
            Marker->SetActorRelativeScale3D(FVector(2.5f));
            Marker->SetActorHiddenInGame(true);
        }
    }
}

// Called every frame
void ARelicsBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARelicsBase::ReduceDustOpacity(const FVector& BrushLocation, float Amount, ABrushTool& BrushRef)
{
    UDecalComponent* Closest = nullptr;
    float MinDist = MAX_flt;

    for (UDecalComponent* Decal : DustDecals)
    {
        if (!Decal || !DecalMIDs.Contains(Decal)) continue;

        float Dist = FVector::Dist(Decal->GetComponentLocation(), BrushLocation);
        if (Dist <= 30.0f && Dist < MinDist)
        {
            MinDist = Dist;
            Closest = Decal;
        }
    }

    if (!Closest) return;

    UMaterialInstanceDynamic* MID = DecalMIDs[Closest];
    float Opacity;
    MID->GetScalarParameterValue(FMaterialParameterInfo(OpacityParameterName), Opacity);
    Opacity = FMath::Clamp(Opacity - Amount, 0.0f, 1.0f);
    MID->SetScalarParameterValue(OpacityParameterName, Opacity);
    
    // 이펙트 업데이트
    ABrushTool* Brush = Cast<ABrushTool>(&BrushRef);
    if (Brush) Brush->UpdateFeedback(Opacity);

    UE_LOG(LogTemp, Log, TEXT("[Debug] Decal Opacity value: %f"), Opacity);

    if (Opacity <= 0.0f)
    {
        Closest->DestroyComponent();
        DustDecals.Remove(Closest);
        DecalMIDs.Remove(Closest);
        DecalToMeshMap.Remove(Closest);

        CheckAllDelcalsRemoved();
		if (Brush) Brush->StopFeedback();
    }
}

void ARelicsBase::CheckAllDelcalsRemoved()
{
    if (DustDecals.Num() == 0)
    {
        UE_LOG(LogTemp, Log, TEXT("[RelicsBase] 모든 데칼 제거 완료!"));

        for (TActorIterator<AExcavationManager> It(GetWorld()); It; ++It)
        {
            It->NotifyDustingCompleted(RelicsManager);
            break;
        }
    }
}

