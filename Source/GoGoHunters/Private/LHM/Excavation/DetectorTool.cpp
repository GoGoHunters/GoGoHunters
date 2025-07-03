// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/Excavation/DetectorTool.h"
#include "LHM/Excavation/RelicsBase.h"
#include "LHM/Excavation/DetectionComponent.h"
#include "LHM/Excavation/DetectionUI.h"
#include "LHM/Excavation/AI_Docent.h"
#include "LHM/Excavation/ExcavationMarker.h"
#include "Components/WidgetComponent.h"
#include "Components/SceneComponent.h"
#include "EngineUtils.h"

// Sets default values
ADetectorTool::ADetectorTool()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	DetectionComp = CreateDefaultSubobject<UDetectionComponent>(TEXT("DetectionComponent"));

	DetectionWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("DetectionWidgetComp"));
	DetectionWidgetComp->SetupAttachment(RootComponent);
	DetectionWidgetComp->SetWidgetSpace(EWidgetSpace::World); // 월드 공간
	DetectionWidgetComp->SetDrawSize(FVector2D(400, 100));    // 원하는 UI 사이즈

	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClassFinder(TEXT("/Game/LHM/UI/WBP_DetectionUI"));
	if (WidgetClassFinder.Succeeded())
	{
		DetectionWidgetComp->SetWidgetClass(WidgetClassFinder.Class);
	}

	static ConstructorHelpers::FClassFinder<AAI_Docent> DocentClassFinder(TEXT("/Game/LHM/BP/Excavation/BP_AI_Docent"));
	if (DocentClassFinder.Succeeded())
	{
		DocentClass = DocentClassFinder.Class;
	}
}

// Called when the game starts or when spawned
void ADetectorTool::BeginPlay()
{
	Super::BeginPlay();
	
	// UI 가시화 테스트용
	// WidgetComponent에 연결된 실제 UDetectionUI 객체 받아오기
	DetectionUI = Cast<UDetectionUI>(DetectionWidgetComp->GetUserWidgetObject());

	// Decent 가시화 테스트용
	if (DocentClass)
	{
		if (!AI_Docent)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			FVector SpawnLoc = GetActorLocation() + FVector(0, 0, 100); // 임의 위치
			AI_Docent = GetWorld()->SpawnActor<AAI_Docent>(DocentClass, SpawnLoc, FRotator::ZeroRotator, SpawnParams);
		}
	}
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

void ADetectorTool::StartDetection()
{
	bIsDetecting = true;
	
	//// 1. 월드 내에서 가장 가까운 RelicsBase를 탐색
	//ARelicsBase* ClosestRelics = nullptr;
	//float ClosestDist = TNumericLimits<float>::Max();
	//
	//for (TActorIterator<ARelicsBase> It(GetWorld()); It; ++It)
	//{
	//	// 1. 마커 존재 확인 (nullptr 체크)
	//	if (!It->Marker) continue;
	//
	//	// 2. 마커가 이미 활성화(=표시 중)면 스킵
	//	if (!It->Marker->IsHidden()) continue;
	//
	//	// 3. 거리를 계산하여 가장 가까운 RelicsBase 찾기
	//	float Dist = FVector::Dist(It->GetActorLocation(), GetActorLocation());
	//	if (Dist < ClosestDist)
	//	{
	//		ClosestDist = Dist;
	//		ClosestRelics = *It;
	//	}
	//}
	//
	//if (ClosestRelics)
	//{
	//	TargetArtifact = ClosestRelics;
	//	bIsDetecting = true;
	//	DetectionProgress = 0.0f;
	//	if (DetectionComp) DetectionComp->OnStartFeedback();
	//}
	//else
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("[DetectorTool] 탐지 가능한 RelicsBase를 찾을 수 없습니다!"));
	//	// 혹시 UI/사운드로 "탐지 불가" 안내도 가능
	//}
}

void ADetectorTool::StopDetection()
{
	bIsDetecting = false;

	// 피드백/이펙트 중지
	if (DetectionComp)
		DetectionComp->StopFeedback();

	// 도슨트 해설 실행
	if (AI_Docent)
		AI_Docent->PlayDetectionComment();

	// 마커 표시
	if (TargetArtifact && TargetArtifact->Marker)
		TargetArtifact->Marker->ActivateMarker();

	// ProgressBar 및 내부 진행도 리셋
	DetectionProgress = 0.f;
	if (DetectionUI)
		DetectionUI->UpdateUI(DetectionProgress);

	UE_LOG(LogTemp, Log, TEXT("[DetectorTool] 탐지 완료 및 UI & 탐지 상태 초기화"));

	// 탐지 상태 초기화
	bIsDetecting = true;
	TargetArtifact = nullptr;
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
		DetectionProgress = 0.f;
		if (DetectionUI)
			DetectionUI->UpdateUI(DetectionProgress);
		return;
	}

	TargetArtifact = ClosestRelics;

	// 3. 진행도 로직
	float Distance = FVector::Dist(GetActorLocation(), TargetArtifact->GetLocation());
	const float MinDetectDistance = 100;
	const float MaxDetectDistance = 800.f;
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
		// 한계 바깥(>8m)은 0%
		DetectionProgress = 0.f;
	}

	if (DetectionComp && DetectionUI)
	{
		DetectionComp->UpdateFeedback(DetectionProgress);
		DetectionUI->UpdateUI(DetectionProgress);
	}

	if (DetectionProgress >= 100.f)
	{
		StopDetection();
	}
}

