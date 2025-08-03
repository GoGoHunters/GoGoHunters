// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/Excavation/RelicsBase.h"
#include "LHM/Excavation/ExcavationMarker.h"
#include "Components/DecalComponent.h"
#include "LHM/Excavation/ExcavationManager.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "LHM/Excavation/BrushTool.h"
#include "LHM/UI/BrushingUI.h"
#include "Components/BoxComponent.h"

// Sets default values
ARelicsBase::ARelicsBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));

    for (int i = 0; i < 8; ++i)
    {
        FString MeshName = FString::Printf(TEXT("RelicMesh_%d"), i + 1);
        UStaticMeshComponent* RelicMesh = CreateDefaultSubobject<UStaticMeshComponent>(*MeshName);
        RelicMesh->SetupAttachment(RootComponent);
        RelicMesh->SetCollisionProfileName(FName("Relic_Buried"));
        RelicMesh->SetGenerateOverlapEvents(true);
        RelicsMeshes.Add(RelicMesh);

        /*int DecalCount = (i == 0) ? 2 : 1;

        for (int j = 0; j < DecalCount; ++j)
        {
            FString DecalName = FString::Printf(TEXT("DustDecal_%d_%d"), i + 1, j + 1);
            UDecalComponent* Decal = CreateDefaultSubobject<UDecalComponent>(*DecalName);
            Decal->SetupAttachment(RelicMesh);
			Decal->DecalSize = FVector(10.0f, 10.0f, 10.0f);
            DustDecals.Add(Decal);
            DecalToMeshMap.Add(Decal, RelicMesh);
        }*/
    }

	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
	TriggerVolume->SetupAttachment(RootComponent);
    TriggerVolume->SetBoxExtent(FVector(1000,1000,100));
    TriggerVolume->SetRelativeLocation(FVector(0,0,-100));
    TriggerVolume->SetGenerateOverlapEvents(true);
}

void ARelicsBase::PostInitializeComponents()
{
    Super::PostInitializeComponents();

	TotalInitialOpacity = 0.f;
	TotalRemainingOpacity = 0.f;

    TArray<UDecalComponent*> AllDecals;
    GetComponents(AllDecals);

    DustDecals.Empty();
    DecalToMeshMap.Empty();
    DecalMIDs.Empty();

    for (UDecalComponent* Decal : AllDecals)
    {
        // 1. DustDecals 배열에 추가
        DustDecals.Add(Decal);

        // 2. DecalToMeshMap 할당
        UStaticMeshComponent* ParentMesh = Cast<UStaticMeshComponent>(Decal->GetAttachParent());
        if (ParentMesh)
        {
            DecalToMeshMap.Add(Decal, ParentMesh);
        }

        // 3. DecalMIDs 할당
        if (Decal->GetMaterial(0))
        {
            UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(Decal->GetMaterial(0), this);
            Decal->SetMaterial(0, MID);
            MID->SetScalarParameterValue(OpacityParameterName, CurrentOpacity);
            DecalMIDs.Add(Decal, MID);

            TotalInitialOpacity += 1.0f;
            TotalRemainingOpacity += 1.0f;
        }
    }

    /*for (UDecalComponent* Decal : DustDecals)
    {
        UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(Decal->GetMaterial(0), this);
        Decal->SetMaterial(0, MID);
        MID->SetScalarParameterValue(OpacityParameterName, CurrentOpacity);
        DecalMIDs.Add(Decal, MID);
        TotalInitialOpacity += 1.0f;
        TotalRemainingOpacity += 1.0f;
    }*/
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
            Marker->SetActorLocation(GetActorLocation() + FVector(-70, 0, 11)); // (X=-70.000000,Y=0.000000,Z=11.000000)
            Marker->SetActorRotation(GetActorRotation()+FRotator(0,-90,0));
            Marker->SetActorRelativeScale3D(FVector(2.0f));
            Marker->SetActorHiddenInGame(true);
        }
    }

    TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &ARelicsBase::OnOverlapBegin);
}

// Called every frame
void ARelicsBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARelicsBase::OnOverlapBegin(UPrimitiveComponent* Overlapped, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{
    if(RelicsMeshes.Num() <= 0) return;

    for(auto* relicMesh : RelicsMeshes)
    {
        if (OtherComp == relicMesh)
        {
			relicMesh->SetWorldLocation(TriggerVolume->GetComponentLocation() + FVector(0, 0, 200));
            relicMesh->SetSimulatePhysics(true);
            relicMesh->SetCollisionProfileName(FName("Relic_Physics"));
            relicMesh->SetGenerateOverlapEvents(true);
            break;
        }
	}
}

void ARelicsBase::ActivateMarker()
{
	if (Marker) Marker->ActivateMarker();
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

    // 데칼 투명도 설정 및 BrushingUI 업데이트
    UMaterialInstanceDynamic* MID = DecalMIDs[Closest];
    float Current;
    MID->GetScalarParameterValue(FMaterialParameterInfo(OpacityParameterName), Current);
    //Opacity = FMath::Clamp(Opacity - Amount, 0.0f, 1.0f);
    float NewOpacity = FMath::Clamp(Current - Amount, 0.0f, 1.0f);
    MID->SetScalarParameterValue(OpacityParameterName, NewOpacity);
	//if (BrushingUI) BrushingUI->UpdateDecalProgress(Closest, Opacity);
    
    // 제거된 양만큼 남은 총합 감소
    TotalRemainingOpacity -= (Current - NewOpacity);
    TotalRemainingOpacity = FMath::Clamp(TotalRemainingOpacity, 0.0f, TotalInitialOpacity);

    // BrushingUI에 전체 진행률 업데이트
    if (BrushingUI && TotalInitialOpacity > 0.0f)
    {
        float Progress = TotalRemainingOpacity / TotalInitialOpacity;
        BrushingUI->UpdateProgress(Progress);
    }

    // 이펙트 업데이트
    ABrushTool* Brush = Cast<ABrushTool>(&BrushRef);
    if (Brush) Brush->UpdateFeedback(Current);

    //UE_LOG(LogTemp, Log, TEXT("[Debug] Decal Opacity value: %f"), Opacity);

    // 데칼 제거
    if (Current <= 0.0f)
    {
        Closest->DestroyComponent();
        DustDecals.Remove(Closest);
        DecalMIDs.Remove(Closest);
        DecalToMeshMap.Remove(Closest);

		UGameplayStatics::PlaySound2D(GetWorld(), DecalRemovalSFX);

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

void ARelicsBase::SetBrushingUI(class UBrushingUI* InBrushingUI)
{
    BrushingUI = InBrushingUI;
    //if (BrushingUI)
    //{
    //    BrushingUI->CreateDecalWidgets(DustDecals);
    //    // 초기 오파시티로 UI 갱신
    //    for (UDecalComponent* Decal : DustDecals)
    //    {
    //        if (Decal && DecalMIDs.Contains(Decal))
    //        {
    //            float Opacity = 1.0f;
    //            DecalMIDs[Decal]->GetScalarParameterValue(FMaterialParameterInfo(OpacityParameterName), Opacity);
    //            BrushingUI->UpdateDecalProgress(Decal, Opacity);
    //        }
    //    }
    //}
}

UStaticMeshComponent* ARelicsBase::GetRelicMeshByDecal(UDecalComponent* Decal) const
{
    const UDecalComponent* const* Found = DustDecals.FindByPredicate(
        [Decal](const UDecalComponent* D) { return D == Decal; });
    return Found ? DecalToMeshMap.FindRef(Decal) : nullptr;
}

bool ARelicsBase::HasValidMID(UDecalComponent* Decal) const
{
    return Decal && DecalMIDs.Contains(Decal);
}

float ARelicsBase::GetDecalOpacity(UDecalComponent* Decal) const
{
    if (HasValidMID(Decal))
    {
        float Opacity = 1.0f;
        DecalMIDs[Decal]->GetScalarParameterValue(FMaterialParameterInfo(OpacityParameterName), Opacity);
        return Opacity;
    }
    return 1.0f;
}

