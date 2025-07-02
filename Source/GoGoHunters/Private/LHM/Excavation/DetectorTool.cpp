// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/Excavation/DetectorTool.h"
#include "LHM/Excavation/RelicsBase.h"
#include "LHM/Excavation/DetectionComponent.h"
#include "LHM/Excavation/DetectionUI.h"
#include "LHM/Excavation/AI_Docent.h"
#include "LHM/Excavation/ExcavationMarker.h"

// Sets default values
ADetectorTool::ADetectorTool()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DetectionComp = CreateDefaultSubobject<UDetectionComponent>(TEXT("DetectionComponent"));

	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClassFinder(TEXT("/Game/LHM/UI/WBP_DetectionUI"));
	if (WidgetClassFinder.Succeeded())
	{
		DetectionUIClass = WidgetClassFinder.Class;
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
	if (DetectionUIClass)
	{
		DetectionUI = CreateWidget<UDetectionUI>(GetWorld()->GetFirstPlayerController(), DetectionUIClass);
		if (DetectionUI)
		{
			DetectionUI->AddToViewport();
		}
	}

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

	if (bIsDetecting && TargetArtifact)
	{
		UpdateDetection(DeltaTime);
	}
}

void ADetectorTool::StartDetection(ARelicsBase* InTarget)
{
	TargetArtifact = InTarget;
	bIsDetecting = true;
	DetectionProgress = 0.0f;
	DetectionComp->OnStartFeedback();
}

void ADetectorTool::StopDetection(class ARelicsBase* InTarget)
{

}

void ADetectorTool::UpdateDetection(float DeltaTime)
{
	float Distance = FVector::Dist(GetActorLocation(), TargetArtifact->GetLocation());

	// 거리가 가까울수록 DetectionSpeed가 커짐 (최소값 보장)
	float DetectionSpeed = GetDetectionSpeed(Distance);

	DetectionProgress += DetectionSpeed * DeltaTime;
	DetectionProgress = FMath::Clamp(DetectionProgress, 0.f, 100.f);

	// 피드백/UI 갱신
	DetectionComp->UpdateFeedback(DetectionProgress);
	DetectionUI->UpdateUI(DetectionProgress);
	
	if (DetectionProgress >= 100.f)
	{
		bIsDetecting = false;
		DetectionComp->StopFeedback();

		// 탐지 완료 처리
		if(AI_Docent) AI_Docent->PlayDetectionComment();
		if(TargetArtifact->Marker) TargetArtifact->Marker->ActivateMarker();
	}
}

float ADetectorTool::GetDetectionSpeed(float Distance) const
{
	// 가까울수록 빠름, 멀면 느림 (튜닝값 예시)
	const float MinSpeed = 2.0f;
	const float MaxSpeed = 20.0f;
	const float MaxDetectionDistance = 300.0f; // 3m

	float Alpha = FMath::Clamp(1.0f - (Distance / MaxDetectionDistance), 0.f, 1.f);
	return MinSpeed + (MaxSpeed - MinSpeed) * Alpha;
}

