// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/Excavation/RelicsGround.h"
//#include "Engine/TextureRenderTarget2D.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Kismet/GameplayStatics.h"
#include "LHM/Excavation/ShovelTool.h"
#include "LHM/Excavation/RelicsManager.h"
#include "Engine/TextureRenderTarget2D.h"
//#include "Kismet/KismetRenderingLibrary.h"

// Sets default values
ARelicsGround::ARelicsGround()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ARelicsGround::BeginPlay()
{
	Super::BeginPlay();

	//SetActorHiddenInGame(true);
}

// Called every frame
void ARelicsGround::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARelicsGround::SetShovelReference(class AShovelTool* NewShovelRef)
{
	Shovel_Ref = NewShovelRef;

	if (Shovel_Ref) OnGroundDug.AddDynamic(Shovel_Ref, &AShovelTool::UpdateFeedback);
}

void ARelicsGround::TriggerOnGroundDug(FVector ImpactLocation)
{
	OnGroundDug.Broadcast(ImpactLocation);

	if (Shovel_Ref) Shovel_Ref->SetIsDigging(false);

	// 파괴량 측정은 한 프레임 지연 후
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ARelicsGround::UpdateDigProgress, 0.1f, false);
}

void ARelicsGround::UpdateDigProgress()
{
	if (!RelicsManager) return;

	float DigProgress = CalculateDestructionFromRenderTarget();
	RelicsManager->NotifyGroundProgress(DigProgress);
}

float ARelicsGround::CalculateDestructionFromRenderTarget()
{
	if (!HeightFieldRT) return 0.f;

	FTextureRenderTargetResource* RTResource = HeightFieldRT->GameThread_GetRenderTargetResource();
	TArray<FColor> Pixels;
	RTResource->ReadPixels(Pixels);

	// 평균 밝기 계산
	int64 TotalR = 0;
	for (const FColor& Pixel : Pixels)
		TotalR += Pixel.R;

	float AvgR = static_cast<float>(TotalR) / Pixels.Num(); // 0~255
	float DestructionPercent = AvgR / 255.f;

	//UE_LOG(LogTemp, Log, TEXT("[RelicsGround] 파괴도: %.2f%%"), DestructionPercent * 100.f);
	return DestructionPercent * 100.f;
}
