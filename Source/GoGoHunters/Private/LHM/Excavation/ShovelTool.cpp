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
}

void AShovelTool::SetIsDigging(bool bNewIsDigging)
{
	bIsDigging = bNewIsDigging;
	
	if (!bIsDigging)
	{
		bWasDiggingLastFrame = false;
		bCanTriggerDigTrace = false;
		bDigActionCompleted = false;
		DigActionTimer = 0.0f;
		ResetDigPattern();
		UE_LOG(LogTemp, Log, TEXT("[Shovel] bIsDiggin false"));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("[Shovel] bIsDiggin true"));
	}
}

void AShovelTool::UpdateDigSwingState(float DeltaTime)
{
	if (bIsDigging)
	{
		// 찔르기 동작 완료 후 쿨다운 처리
		if (bDigActionCompleted)
		{
			DigActionTimer += DeltaTime;
			if (DigActionTimer >= DigActionCooldown)
			{
				bDigActionCompleted = false;
				DigActionTimer = 0.0f;
			}
			else
			{
				bCanTriggerDigTrace = false;
				return;
			}
		}

		// 새로운 땅파기 패턴 감지
		UpdateDigPatternState(DeltaTime);
	}
	else
	{
		if (bWasDiggingLastFrame)
		{
			bWasDiggingLastFrame = false;
			bCanTriggerDigTrace = false;
			ResetDigPattern();
		}
	}
}

void AShovelTool::UpdateDigPatternState(float DeltaTime)
{
	AMH_VRPlayer* VRPlayer = Cast<AMH_VRPlayer>(this->GetAttachParentActor());
	if (!VRPlayer) return;

	UMotionControllerComponent* HandController = VRPlayer->RHandController;
	if (!HandController) return;
	if (!SplatPoint) return;

	if (!bWasDiggingLastFrame)
	{
		PreviousLocation = SplatPoint->GetComponentLocation();
		bWasDiggingLastFrame = true;
		UE_LOG(LogTemp, Verbose, TEXT("[Shovel] Init previous location for pattern calc (SplatPoint)"));
		return; // 첫 프레임은 계산 생략
	}

	FVector CurrentLocation = SplatPoint->GetComponentLocation();
	FVector Velocity = (CurrentLocation - PreviousLocation) / DeltaTime;
	PreviousLocation = CurrentLocation;

	// 현재 상태에 따른 처리
	switch (CurrentDigState)
	{
	case EDigPatternState::Idle:
		// 내리꽂기 동작 감지
		if (EvaluateStabbingMotion(CurrentLocation, Velocity))
		{
			CurrentDigState = EDigPatternState::Stabbing;
			StabStartLocation = CurrentLocation;
			StabStartTime = GetWorld()->GetTimeSeconds();
			bReachedMinDepth = false;
			UE_LOG(LogTemp, Log, TEXT("[Shovel] -> Stabbing | StartZ=%.1f"), StabStartLocation.Z);
		}
		break;

	case EDigPatternState::Stabbing:
		// 시간 초과 체크
		if (GetWorld()->GetTimeSeconds() - StabStartTime > MaxStabTime)
		{
			ResetDigPattern();
			return;
		}

		//// 깊이 체크
		//float CurrentDepth = StabStartLocation.Z - CurrentLocation.Z;
		if ((StabStartLocation.Z - CurrentLocation.Z) >= MinStabDepth)
		{
			bReachedMinDepth = true;
			UE_LOG(LogTemp, Log, TEXT("[Shovel] Reached MinStabDepth | Depth=%.1f / Min=%.1f"), (StabStartLocation.Z - CurrentLocation.Z), MinStabDepth);
		}

		// 위로 퍼내기 동작 감지
		if (bReachedMinDepth && EvaluateLiftingMotion(CurrentLocation, Velocity))
		{
			CurrentDigState = EDigPatternState::Lifting;
			LiftStartTime = GetWorld()->GetTimeSeconds();
			UE_LOG(LogTemp, Log, TEXT("[Shovel] -> Lifting"));
		}
		break;

	case EDigPatternState::Lifting:
		// 시간 초과 체크
		if (GetWorld()->GetTimeSeconds() - LiftStartTime > MaxLiftTime)
		{
			ResetDigPattern();
			return;
		}

		// 충분히 위로 올라갔는지 체크
		float LiftHeight = CurrentLocation.Z - StabStartLocation.Z;
		if (LiftHeight >= MinLiftHeight)
		{
			// 땅파기 성공!
			bCanTriggerDigTrace = true;
			ResetDigPattern();
			UE_LOG(LogTemp, Log, TEXT("[Shovel] Dig SUCCESS | LiftHeight=%.1f / Min=%.1f"), LiftHeight, MinLiftHeight);
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("[Shovel] Dig FAILED | LiftHeight=%.1f / Min=%.1f"), LiftHeight, MinLiftHeight);
		}
		break;
	}
}

bool AShovelTool::EvaluateStabbingMotion(const FVector& CurrentLocation, const FVector& Velocity)
{
	// 아래 + 앞 방향 동시 체크
	FVector NormalizedVelocity = Velocity.GetSafeNormal();
	float DownwardDot = FVector::DotProduct(NormalizedVelocity, FVector::DownVector);
	float Speed = Velocity.Size();

	// 컨트롤러 Forward 기준 전방성 확인
	float ForwardDot = 0.0f;
	if (AMH_VRPlayer* VRPlayer = Cast<AMH_VRPlayer>(GetAttachParentActor()))
	{
		if (UMotionControllerComponent* HandController = VRPlayer->RHandController)
		{
			ForwardDot = FVector::DotProduct(NormalizedVelocity, HandController->GetForwardVector());
		}
	}

	// 임계값: 아래 0.5+, 앞 0.5+, 속도 30cm/s+
	bool bPass = (DownwardDot > 0.5f) && (ForwardDot > 0.5f) && (Speed > 30.0f);
	UE_LOG(LogTemp, Log, TEXT("[Shovel][Stab] DownDot=%.2f ForwardDot=%.2f Speed=%.1f -> %s"), DownwardDot, ForwardDot, Speed, bPass ? TEXT("PASS") : TEXT("FAIL"));
	return bPass;
}

bool AShovelTool::EvaluateLiftingMotion(const FVector& CurrentLocation, const FVector& Velocity)
{
	// 위 방향으로 충분히 빠르게 움직이는지 체크
	FVector NormalizedVelocity = Velocity.GetSafeNormal();
	float UpwardDot = FVector::DotProduct(NormalizedVelocity, FVector::UpVector);
	float Speed = Velocity.Size();
	
	// 위 방향으로 0.6 이상, 속도 20cm/s 이상
	bool bPass = (UpwardDot > 0.6f) && (Speed > 60.0f);
	UE_LOG(LogTemp, Log, TEXT("[Shovel][Lift] UpDot=%.2f Speed=%.1f -> %s"), UpwardDot, Speed, bPass ? TEXT("PASS") : TEXT("FAIL"));
	return bPass;
}

void AShovelTool::ResetDigPattern()
{
	CurrentDigState = EDigPatternState::Idle;
	bReachedMinDepth = false;
	StabStartTime = 0.0f;
	LiftStartTime = 0.0f;
}

void AShovelTool::OnDigActionCompleted()
{
	bDigActionCompleted = true;
	DigActionTimer = 0.0f;
	bCanTriggerDigTrace = false;
}

void AShovelTool::UpdateFeedback(FVector ImpactLocation)
{
	if (!bIsDigging) return;

	// 찔르기 동작 완료 처리
	OnDigActionCompleted();

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
