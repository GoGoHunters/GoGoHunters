// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/Excavation/DetectorTool.h"
#include "LHM/Excavation/RelicsBase.h"
#include "LHM/Excavation/DetectionComponent.h"
#include "LHM/Excavation/ExcavationMarker.h"
#include "LHM/UI/DetectionUI.h"
#include "Components/WidgetComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EngineUtils.h"
#include "LHM/Excavation/RelicsGround.h"
#include "LHM/Excavation/RelicsManager.h"
#include "LHM/Excavation/ExcavationManager.h"

// Sets default values
ADetectorTool::ADetectorTool()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	DetectorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DetectorMesh"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Game/LHM/Meshes/SM_DetectionTool.SM_DetectionTool"));
	if (MeshAsset.Succeeded())
	{
		DetectorMesh->SetStaticMesh(MeshAsset.Object);
		DetectorMesh->SetupAttachment(RootComponent);
	}

	DetectionComp = CreateDefaultSubobject<UDetectionComponent>(TEXT("DetectionComponent"));

	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClassFinder(TEXT("/Game/LHM/UI/WBP_DetectionUI"));
	if (WidgetClassFinder.Succeeded())
	{
		DetectionWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("DetectionWidgetComp"));
		DetectionWidgetComp->SetWidgetClass(WidgetClassFinder.Class);
		DetectionWidgetComp->SetupAttachment(DetectorMesh);
		DetectionWidgetComp->SetWidgetSpace(EWidgetSpace::World); // 월드 공간
		DetectionWidgetComp->SetRelativeLocation(FVector(3, 0, 2.01)); // (X=3.000000,Y=0.000000,Z=2.010000)
		DetectionWidgetComp->SetRelativeRotation(FRotator(FRotator(90, 180, 0))); // (Pitch=90.000000,Yaw=180.000000,Roll=0.000000)
		DetectionWidgetComp->SetRelativeScale3D(FVector(0.1));
		DetectionWidgetComp->SetDrawSize(FVector2D(300, 90));
	} 
}

// Called when the game starts or when spawned
void ADetectorTool::BeginPlay()
{
	Super::BeginPlay();
	
	// UI 가시화 테스트용
	DetectionUI = Cast<UDetectionUI>(DetectionWidgetComp->GetUserWidgetObject());

}

// Called every frame
void ADetectorTool::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsDetecting)
	{
		UpdateDetection(DeltaTime);
	}
}

void ADetectorTool::SetIsDetecting(bool _bIsDetecting)
{
	bIsDetecting = _bIsDetecting;

	if (!bIsDetecting)
	{
		StopDetection();
	}
}

void ADetectorTool::StopDetection()
{
	bIsDetecting = false;

	// 피드백/이펙트 중지
	if (DetectionComp)
	{
		DetectionComp->StopFeedback();
	}

	// ProgressBar 및 내부 진행도 리셋
	if (DetectionUI)
	{
		DetectionProgress = 0.f;
		DetectionUI->UpdateUI(DetectionProgress);
	}
}

void ADetectorTool::UpdateDetection(float DeltaTime)
{
// 1. 가장 가까운 탐지 가능한 Relics 찾기
	ARelicsBase* ClosestRelics = nullptr;
	float ClosestDist = TNumericLimits<float>::Max();

	for (TActorIterator<ARelicsBase> It(GetWorld()); It; ++It)
	{
		if (!It->Marker) continue;
		if (!It->Marker->IsHidden()) // 이미 탐지 완료된 유물은 건너뜀
			continue;

		float Dist = FVector::Dist(It->GetActorLocation(), GetActorLocation());
		if (Dist < ClosestDist)
		{
			ClosestDist = Dist;
			ClosestRelics = *It;
		}
	}

// 2. 탐지 가능한 Relics가 없으면 UI 리셋, 진행도 0
	if (!ClosestRelics)
	{
		if (DetectionUI)
		{
			DetectionProgress = 0.f;
			DetectionUI->UpdateUI(DetectionProgress);
		}
		return;
	}

	Relics = ClosestRelics;

// 3. 진행도 로직
	float Distance = FVector::Dist(GetActorLocation(), Relics->GetActorLocation());
	const float MinDetectDistance = 500; // 테스트용
	//const float MinDetectDistance = 300;
	const float MaxDetectDistance = 1500.f;
	const float FillSpeed = 50.f;

	if (Distance > MinDetectDistance && Distance <= MaxDetectDistance)
	{
		// 70%까지는 거리 기반 즉시 반영
		float Ratio = 1.f - (Distance - MinDetectDistance) / (MaxDetectDistance - MinDetectDistance);
		float TargetProgress = FMath::Clamp(Ratio * 70.f, 0.f, 70.f);
		DetectionProgress = TargetProgress;
	}
	else if (Distance <= MinDetectDistance)
	{
		// 30cm 이내로 들어왔으면 진행률이 서서히 차오름 (70~100%)
		DetectionProgress += FillSpeed * DeltaTime;
		DetectionProgress = FMath::Clamp(DetectionProgress, 70.f, 100.f);
	}
	else
	{
		// 한계 바깥(>15m)은 0%
		DetectionProgress = 0.f;
	}

	if (DetectionComp && DetectionUI)
	{
		DetectionComp->UpdateFeedback(DetectionProgress);
		DetectionUI->UpdateUI(DetectionProgress);
	}

	if (DetectionProgress >= 100.f)
	{
		// 해당 Relics를 관리하는 Manager 찾기
		ARelicsManager* FindManager = nullptr;
		for (TActorIterator<ARelicsManager> It(GetWorld()); It; ++It)
		{
			if (It->GetRelics() == Relics)
			{
				FindManager = *It;
				break;
			}
		}

		if (FindManager)
		{
			// 전역 ExcavationManager를 찾아서 Relics 발굴 시작
			for (TActorIterator<AExcavationManager> It(GetWorld()); It; ++It)
			{
				It->StartRelicsExcavation(FindManager);
				break;
			}
		}

		// 마커 표시
		if (Relics && Relics->Marker)
		{
			Relics->Marker->ActivateMarker();
			Relics = nullptr;
		}

		UE_LOG(LogTemp, Log, TEXT("[DetectorTool] 탐지 완료 및 UI & 탐지 상태 초기화"));

		StopDetection(); // 탐지 완료 후 상태 초기화
	}
}

