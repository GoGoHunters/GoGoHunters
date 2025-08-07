// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/Excavation/ShovelTool.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Blueprint/UserWidget.h"
#include "LHM/Excavation/RelicsGround.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "JMH/MH_VRPlayer.h"
#include "MotionControllerComponent.h"
#include "../../../../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraFunctionLibrary.h"

// Sets default values
AShovelTool::AShovelTool()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	ShovelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShovelMesh"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ShovelMeshAsset(TEXT("/Game/JMH/Mesh/04_Assets/Tools/Shovels0212.Shovels0212"));
	if (ShovelMeshAsset.Succeeded())
	{
		ShovelMesh->SetStaticMesh(ShovelMeshAsset.Object);
		ShovelMesh->SetupAttachment(RootComponent);
		ShovelMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		ShovelMesh->bReceivesDecals = false;
	}

	SplatPoint = CreateDefaultSubobject<USceneComponent>(TEXT("SplatPoint"));
	SplatPoint->SetupAttachment(ShovelMesh);
	SplatPoint->SetRelativeLocation(FVector(165.0, 0, 0));

	bIsDigging = false;
}

// Called when the game starts or when spawned
void AShovelTool::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AShovelTool::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateDigSwingState(DeltaTime);

	/*if (bIsDigHoldState)
	{
		HoldTimer += DeltaTime;
		EvaluateShovelLiftMotion(DeltaTime);
	}
	else
	{
		if(HoldTimer > MaxHoldTime) bIsDigHoldState = false;
	}*/
}

void AShovelTool::UpdateDigSwingState(float DeltaTime)
{
	if (bIsDigging)
	{
		AMH_VRPlayer* VRPlayer = Cast<AMH_VRPlayer>(this->GetAttachParentActor());
		if (VRPlayer)
		{
			UMotionControllerComponent* HandController = VRPlayer->RHandController;

			if (!bWasDiggingLastFrame)
			{
				PreviousLocation = HandController->GetComponentLocation();
				bWasDiggingLastFrame = true;
				return; // 첫 프레임은 계산 생략
			}

			FVector CurrentLocation = HandController->GetComponentLocation();

			FVector Velocity = (CurrentLocation - PreviousLocation) / DeltaTime;
			PreviousLocation = CurrentLocation;

			FVector NormalizedVelocity = Velocity.GetSafeNormal();

			bool bIsMovingDownward = FVector::DotProduct(NormalizedVelocity, FVector::DownVector) > 0.5f;				// -Z 방향
			bool bIsMovingForward = FVector::DotProduct(NormalizedVelocity, HandController->GetForwardVector()) > 0.5f;	// X 방향

			float SwingThreshold = 100.0f;
			bool bFastEnough = Velocity.Size() > SwingThreshold;
			bCanTriggerDigTrace = bFastEnough && bIsMovingDownward && bIsMovingForward;

			UE_LOG(LogTemp, Log, TEXT("Speed: %.1f | DownDot: %.2f | ForwardDot: %.2f"),
				   Velocity.Size(),
				   FVector::DotProduct(NormalizedVelocity, FVector::DownVector),
				   FVector::DotProduct(NormalizedVelocity, VRPlayer->GetActorForwardVector()));
		}
	}
	else
	{
		if (bWasDiggingLastFrame)
		{
			bWasDiggingLastFrame = false;
			bCanTriggerDigTrace = false;
		}

		/*if (bWasLiftingLastFrame)
		{
			bWasLiftingLastFrame = false;
			bIsShovelLifting = false;
		}*/
	}
}

//void AShovelTool::EvaluateShovelLiftMotion(float DeltaTime)
//{
//	if(!bIsDigging || !bCanTriggerDigTrace || !SplatPoint)
//	{
//		bIsShovelLifting = false;
//		bWasDiggingLastFrame = false;
//		return;
//	}
//
//	if(!bWasLiftingLastFrame)
//	{
//		PreviousSplatLocation = SplatPoint->GetComponentLocation();
//		bWasLiftingLastFrame = true;
//		bIsShovelLifting = false;
//		return; // 첫 프레임은 계산 생략
//	}
//
//	// 위치 기준 속도 계산
//	FVector CurrentLocation = SplatPoint->GetComponentLocation();
//	FVector Velocity = (CurrentLocation - PreviousSplatLocation) / DeltaTime;
//	PreviousSplatLocation = CurrentLocation;
//
//	// 방향 일치 판단
//	float Speed = Velocity.Size();
//	FVector NormalizedVelocity = Velocity.GetSafeNormal();
//	FVector UpDirection = SplatPoint->GetUpVector();
//
//	//float Dot = FVector::DotProduct(NormalizedVelocity, UpDirection);
//	float Dot = NormalizedVelocity.Z;
//
//	// Dot 값이 0.5 이상이고 속도가 100 이상일 때만 리프팅으로 간주
//	bIsShovelLifting = Dot > 0.5f && Speed > 100.0f;
//
//	//UE_LOG(LogTemp, Log, TEXT("[LiftCheck] Dot: %.2f, Speed: %.1f"), Dot, Speed);
//	UE_LOG(LogTemp, Log, TEXT("Speed: %.2f | Dot: %.2f | Velocity: %s | Up: %s"),
//		   Speed, Dot, *NormalizedVelocity.ToString(), *UpDirection.ToString());
//}

void AShovelTool::UpdateFeedback(FVector ImpactLocation)
{
	if (!bIsDigging) return;

	// 햅틱 피드백 재생
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC && DigHapticFX)
	{ 
		PC->PlayHapticEffect(DigHapticFX, EControllerHand::Right, 1.0f, false);
	}

	// 나이아가라
	if (DigFX)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			DigFX,
			ImpactLocation,
			FRotator::ZeroRotator,
			FVector(1.0f),
			true, true, ENCPoolMethod::AutoRelease, true
		);
	}

	// 사운드 재생
	if (SoundFX)
	{
		UGameplayStatics::PlaySoundAtLocation(this, SoundFX, ImpactLocation);
	}
}
