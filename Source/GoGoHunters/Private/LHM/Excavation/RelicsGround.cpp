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

	if (Shovel_Ref) OnGroundDug.AddDynamic(Shovel_Ref, &AShovelTool::PlayFeedback);
}

void ARelicsGround::TriggerOnGroundDug(FVector ImpactLocation)
{
	OnGroundDug.Broadcast(ImpactLocation);

	// 파괴량 측정은 한 프레임 지연 후
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ARelicsGround::UpdateDigProgress, 0.1f, false);
}

void ARelicsGround::UpdateDigProgress()
{
	if (!RelicsManager) return;

	float DigProgress = CalculateDestructionFromRenderTarget();
	//RelicsManager->NotifyGroundProgress(DigProgress);
}

float ARelicsGround::CalculateDestructionFromRenderTarget()
{
	//if (!PainterMaterial) return 0.f;

	//UTexture* Tex = nullptr;
	//PainterMaterial->GetTextureParameterValue(FMaterialParameterInfo("HeightField"), Tex);

	////UTextureRenderTarget2D* HeightRT = Cast<UTextureRenderTarget2D>(Tex);
	//HeightFieldRT = Cast<UTextureRenderTarget2D>(Tex);
	//if (!HeightFieldRT) return 0.f;

	//FTextureRenderTargetResource* RTResource = HeightFieldRT->GameThread_GetRenderTargetResource();
	//TArray<FColor> Pixels;
	//RTResource->ReadPixels(Pixels);

	//int32 DestroyedPixels = 0;
	//for (const FColor& Pixel : Pixels)
	//{
	//	if (Pixel.R < 100) DestroyedPixels++;
	//}

	////UE_LOG(LogTemp, Log, TEXT("[RelicsGround] Destruction Percent: %.2f%%"), DestroyedPixels / Pixels.Num());
	//return (float)DestroyedPixels / Pixels.Num();

	if (!HeightFieldRT)
	{
		UE_LOG(LogTemp, Warning, TEXT("RenderTarget 없음"));
		return 0.f;
	}

	FTextureRenderTargetResource* RTResource = HeightFieldRT->GameThread_GetRenderTargetResource();
	TArray<FColor> Pixels;
	RTResource->ReadPixels(Pixels);

	if (Pixels.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[RelicsGround] 픽셀 없음"));
		return 0.f;
	}

	int32 Destroyed = 0;
	for (const FColor& Pixel : Pixels)
	{
		if (Pixel.R < 100) Destroyed++;
	}

	float Percent = (float)Destroyed / Pixels.Num();

	UE_LOG(LogTemp, Log, TEXT("[RelicsGround] 파괴도: %.2f%% (총 픽셀: %d, 파괴된 픽셀: %d)"),
		   Percent * 100.f, Pixels.Num(), Destroyed);
	return Percent;
}
