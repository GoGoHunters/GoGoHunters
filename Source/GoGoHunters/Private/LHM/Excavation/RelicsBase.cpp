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

    //UE_LOG(LogTemp, Log, TEXT("[PostInitializeComponents] DustDecals size: %d"), DustDecals.Num());
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

UStaticMeshComponent* ARelicsBase::GetClosestRelicMesh(const FVector& BrushLocation) const
{
    UStaticMeshComponent* ClosestMesh = nullptr;
    float MinDist = FLT_MAX;

    for (UStaticMeshComponent* Mesh : RelicsMeshes)
    {
        if (!Mesh) continue;

        float Dist = FVector::Dist(BrushLocation, Mesh->GetComponentLocation());
        if (Dist < MinDist)
        {
            MinDist = Dist;
            ClosestMesh = Mesh;
        }
    }

    return ClosestMesh;
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
    float NewOpacity = FMath::Clamp(Current - Amount, 0.0f, 1.0f);
    if (NewOpacity <= 0.2f) NewOpacity = 0.0f;
	MID->SetScalarParameterValue(OpacityParameterName, NewOpacity);
    
    // 제거된 양만큼 남은 총합 감소
    TotalRemainingOpacity -= (Current - NewOpacity);
    TotalRemainingOpacity = FMath::Clamp(TotalRemainingOpacity, 0.0f, TotalInitialOpacity);

    // BrushingUI에 전체 진행률 업데이트
    if (BrushingUI && TotalInitialOpacity > 0.0f)
    {
        float Progress = TotalRemainingOpacity / TotalInitialOpacity;

        if (Progress >= Progress * 0.5f)
        {
            if(!bIsPlayingTami) PlayTami();
        }

        BrushingUI->UpdateProgress(Progress);
    }

    // 이펙트 업데이트
    ABrushTool* Brush = Cast<ABrushTool>(&BrushRef);
    if (Brush) Brush->UpdateFeedback(Current);

    //UE_LOG(LogTemp, Log, TEXT("[Debug] Decal Opacity value: %f"), Opacity);

    // 데칼 제거
    if (NewOpacity <= 0.2f)
    {
        Closest->DestroyComponent();

        // [1] Decal 제거 및 관련 맵 정리
        UStaticMeshComponent* ParentMesh = DecalToMeshMap.FindRef(Closest);
        DustDecals.Remove(Closest);
        DecalMIDs.Remove(Closest);
        DecalToMeshMap.Remove(Closest);

        // [2] 남은 데칼 중 ParentMesh에 붙어있는 게 있는지 검사
        bool bShouldChangeColor = false;

        if (ParentMesh == RelicsMeshes[0])
        {
			// RelicMesh_1의 경우: 데칼이 모두 제거된 경우에만 색상 변경
            bool bAllRemoved = true;
            for (const auto& Pair : DecalToMeshMap)
            {
                if (Pair.Value == RelicsMeshes[0] && Pair.Key != Closest)
                {
                    bAllRemoved = false;
                    break;
                }
            }

            if (bAllRemoved)
            {
                bShouldChangeColor = true;
            }
        }
        else if (ParentMesh) // RelicMesh_2~8
        {
            bShouldChangeColor = true;
        }

        // [3] 붙어있는 데칼이 모두 제거된 경우만 머티리얼 색상 변경
        if (bShouldChangeColor && ParentMesh)
        {
            UMaterialInterface* MatInterface = ParentMesh->GetOverlayMaterial();
            if (MatInterface)
            {
                UMaterialInstanceDynamic* OverlayMID = Cast<UMaterialInstanceDynamic>(MatInterface);
                if (!OverlayMID)
                {
                    OverlayMID = UMaterialInstanceDynamic::Create(MatInterface, this);
                    ParentMesh->SetOverlayMaterial(OverlayMID);
                }

                // 색상 변경
                OverlayMID->SetVectorParameterValue(FName("Color"), FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
            }
        }

		// [4] 효과음 및 피드백
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

void ARelicsBase::PlayTami()
{
    bIsPlayingTami = true;

    for (TActorIterator<APawn> It(GetWorld(), APawn::StaticClass()); It; ++It)
    {
        if (IsValid(*It) && (*It)->ActorHasTag(FName("Tami")))
        {
            if (APawn* TamiAI = *It)
            {
                FName FunctionName(TEXT("PlayExcavationCompliment"));
                if (UFunction* Function = TamiAI->FindFunction(FunctionName))
                {
                    TamiAI->ProcessEvent(Function, nullptr);
                }
            }
            break;
        }
    }
}

