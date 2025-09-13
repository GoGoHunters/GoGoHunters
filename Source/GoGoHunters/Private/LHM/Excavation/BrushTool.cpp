// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/Excavation/BrushTool.h"
#include "LHM/Excavation/RelicsBase.h"
#include "Components/BoxComponent.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../../../../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraFunctionLibrary.h"
#include "../../../../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h"
#include "EngineUtils.h"
#include "LHM/UI/WarningTextUI.h"

// 속도 기반으로 0~1 정규화 → HSV 보간
static FLinearColor MakeBrushColorFromSpeed(float Speed, float Min, float Max)
{
	const float T = FMath::GetRangePct(Min, Max, Speed); // 0~1
	const FLinearColor Green(0.00f, 1.00f, 0.00f, 1.0f);
	const FLinearColor Red(1.00f, 0.00f, 0.00f, 1.0f);
	return FLinearColor::LerpUsingHSV(Green, Red, FMath::Clamp(T, 0.0f, 1.0f));
}

// Sets default values
ABrushTool::ABrushTool()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	BrushMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BrushMesh"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(TEXT("/Game/JMH/Anim/Brush/Brush_Anim01.Brush_Anim01"));
	if (MeshAsset.Succeeded())
	{
		BrushMesh->SetSkeletalMesh(MeshAsset.Object);
		BrushMesh->SetupAttachment(RootComponent);
		BrushMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		BrushMesh->bReceivesDecals = false;
	}

	BoxMesh = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxMesh"));
	BoxMesh->SetupAttachment(BrushMesh);
	BoxMesh->SetBoxExtent(FVector(16, 25, 12)); // (X=16.000000,Y=25.000000,Z=12.000000)
	BoxMesh->SetRelativeLocation(FVector(70, 0, 8)); // (X=70.000000,Y=-0.000000,Z=8.000000)
	BoxMesh->SetGenerateOverlapEvents(true);
	BoxMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxMesh->SetCollisionObjectType(ECC_WorldDynamic);
	BoxMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	BoxMesh->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);

	BoxMesh->OnComponentBeginOverlap.AddDynamic(this, &ABrushTool::OnBeginOverlap);
	BoxMesh->OnComponentEndOverlap.AddDynamic(this, &ABrushTool::OnEndOverlap);

	SwipeVFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SwipeVFX"));
	SwipeVFX->SetupAttachment(BoxMesh);
	SwipeVFX->SetRelativeScale3D(FVector(2.5f));
	SwipeVFX->bAutoActivate = false;
}

// Called when the game starts or when spawned
void ABrushTool::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ABrushTool::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SwipeSpeed = FVector::Dist(BoxMesh->GetComponentLocation(), PreviousLocation) / DeltaTime;
	PreviousLocation = BoxMesh->GetComponentLocation();

	if (bIsBrushing) CheckBrushSwipe(DeltaTime);
}

void ABrushTool::OnBeginOverlap(UPrimitiveComponent* Overlapped, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 BodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	if (ARelicsBase* RelicRef = Cast<ARelicsBase>(OtherActor))
	{
		CurrentOverlappingRelic = RelicRef;
	}
}

void ABrushTool::OnEndOverlap(UPrimitiveComponent* Overlapped, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 BodyIndex)
{
	if (OtherActor == CurrentOverlappingRelic)
	{
		CurrentOverlappingRelic = nullptr;
	}
}

void ABrushTool::CheckBrushSwipe(float DeltaTime)
{
	if (!CurrentOverlappingRelic) return;
	
	Relic = Cast<ARelicsBase>(CurrentOverlappingRelic);
	if (!Relic) return;

	// 경고 지연 판정 중에는 내려갔는지 감시만 한다
	if (bWarningCheckPending)
	{
		if (SwipeSpeed < BrushSwipeThresholdMax)
		{
			bWarningWindowHadDropBelow = true;
		}
		else
		{
			bWarningWindowHadDropBelow = false;
		}
	}

	// [1] 가장 가까운 메시
	UStaticMeshComponent* ClosestMesh = Relic->GetClosestRelicMesh(BoxMesh->GetComponentLocation());
	if (!ClosestMesh) return;

	// [2] 데칼이 하나라도 남아 있는지 확인
	bool bHasRemainingDecal = false;

	for (const auto& Pair : Relic->DecalToMeshMap)
	{
		if (Pair.Value == ClosestMesh)
		{
			bHasRemainingDecal = true;
			break;
		}
	}

	// [3] 데칼이 남아 있지 않으면 return
	if (!bHasRemainingDecal) return;

	// [4] 속도 기반 Swipe 나이아가라 재생
	if (SwipeSpeed > BrushSwipeThresholdMin)
	{
		UpdateSwipeFeedback(SwipeSpeed);
	}

	// [5] 먼지 털기 실행
	if (SwipeSpeed > BrushSwipeThresholdMin
		&& SwipeSpeed < BrushSwipeThresholdMax)
	{
		Relic->ReduceDustOpacity(BoxMesh->GetComponentLocation(), FadeSpeed * DeltaTime, *this);
	}
	else if (bCanTriggerWarning && !bWarningCheckPending && SwipeSpeed >= BrushSwipeThresholdMax)
	{
		HandleBrushHardSwipeFeedbackAndWarn();
	}
}

void ABrushTool::UpdateFeedback(float Intensity)
{
	PlayVibration(Intensity);
	UpdateDustFeedback(Intensity);
	PlaySoundFeedback(Intensity);
}

void ABrushTool::StopFeedback()
{
	// 햅틱 중지
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC)
	{
		PC->StopHapticEffect(EControllerHand::Right);
	}

	// Swipe 나이아가라 중지
	if (SwipeVFX->IsActive())
	{
		SwipeVFX->Deactivate();
	}
}

void ABrushTool::PlayVibration(float Intensity)
{
	// 햅틱 피드백 재생
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC && HapticEffect)
	{
		float ClampedIntensity = FMath::Clamp(Intensity, 0.0f, 1.0f);
		PC->PlayHapticEffect(HapticEffect, EControllerHand::Right, ClampedIntensity, false);
	}
}

void ABrushTool::UpdateDustFeedback(float Intensity)
{
	if(!BrushFX) return;
	
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(),
		BrushFX,
		BoxMesh->GetComponentLocation(),
		FRotator::ZeroRotator,
		FVector(1.0f),
		true, true, ENCPoolMethod::AutoRelease, true
	);
}

void ABrushTool::UpdateSwipeFeedback(float Speed)
{
	// Swipe 나이아가라
	if (SwipeVFX)
	{
		// 속도→색상 (30 이하면 민트, 200 이상이면 레드, 그 사이는 보간)
		const FLinearColor Color = 
			MakeBrushColorFromSpeed(Speed, BrushSwipeThresholdMin, BrushSwipeThresholdMax);
		SwipeVFX->SetNiagaraVariableLinearColor(TEXT("User.User_BrushColor"), Color);

		if(!SwipeVFX->IsActive()) SwipeVFX->Activate(true);
	}
}

void ABrushTool::PlaySoundFeedback(float Intensity)
{
	if (!SoundEffect) return;

	UWorld* World = GetWorld();
	if (!World) return;

	float CurrentTime = World->GetTimeSeconds();

	// 쿨타임 체크
	if (CurrentTime - LastSoundPlayTime < SoundCooldown) return;

	//float Volume = FMath::Clamp(Intensity, 0.0f, 1.0f);
	UGameplayStatics::PlaySoundAtLocation(
		this,
		SoundEffect,
		GetActorLocation(),
		Intensity
	);

	LastSoundPlayTime = CurrentTime;
}

void ABrushTool::SetIsBrushing(bool _bIsBrushing)
{
	bIsBrushing = _bIsBrushing;

	if (!bIsBrushing) StopFeedback();
}

void ABrushTool::HandleBrushHardSwipeFeedbackAndWarn()
{
	if (!Relic) return;
	// 추가 가드: 중복 호출 방지
	if (bWarningCheckPending || !bCanTriggerWarning) return;

	// 먼저 플래그 설정하여 같은 틱 내 재진입 방지
	bCanTriggerWarning = false;
	bWarningCheckPending = true;
	bWarningWindowHadDropBelow = false;

	// 1) 사운드
	if (HardBrushSFX)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HardBrushSFX, GetActorLocation());
	}

	// 2) 햅틱
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		if (HardBrushHaptic)
		{
			PC->PlayHapticEffect(HardBrushHaptic, EControllerHand::Right);
		}
	}

	// 3) 경고 문구
	if (Relic && Relic->GetWarningTextUI())
	{
		Relic->GetWarningTextUI()->PlayAlertAnim();
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(WarningDelayHandle);
		World->GetTimerManager().SetTimer(
			WarningDelayHandle,
			this,
			&ABrushTool::OnWarningDelayElapsed,
			WarningDelayAfterImpact,
			false
		);
	}
}

void ABrushTool::ResetWarningCooldown()
{
	bCanTriggerWarning = true;
}

void ABrushTool::OnWarningDelayElapsed()
{
	// 2초 경과 후 상태 평가
	const bool bStillOverlapping = (CurrentOverlappingRelic != nullptr);
	//const bool bShouldWarn = bStillOverlapping && (Relic != nullptr) && !bWarningWindowHadDropBelow && (SwipeSpeed >= BrushSwipeThresholdMax);

	/*UE_LOG(LogTemp, Log, TEXT("[BrushTool] Warning Delay Elapsed: StillOverlapping=%d, RelicValid=%d, HadDropBelow=%d, SwipeSpeed=%.2f, ShouldWarn=%d"),
		bStillOverlapping,
		(Relic != nullptr),
		bWarningWindowHadDropBelow,
		SwipeSpeed,
		bShouldWarn
	);*/

	bWarningCheckPending = false;
	bWarningWindowHadDropBelow = false;

	/*if (bShouldWarn)
	{
		// 경고 사운드 및 UI 표시
		if (WarningSFX) UGameplayStatics::PlaySoundAtLocation(this, WarningSFX, GetActorLocation());

		Relic->CountWarning();

		// 쿨타임 시작 (판정 성공 시에만)
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(WarningCooldownHandle);
			World->GetTimerManager().SetTimer(
				WarningCooldownHandle,
				this,
				&ABrushTool::ResetWarningCooldown,
				WarningCooldownDuration,
				false
			);
		}
	}
	else
	{
		// 경고 미표시 → 즉시 재트리거 허용
		bCanTriggerWarning = true;
	}*/

	bCanTriggerWarning = true;
}

void ABrushTool::CancelPendingWarningCheck()
{
	if (!bWarningCheckPending) return;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(WarningDelayHandle);
	}

	bWarningCheckPending = false;
	bWarningWindowHadDropBelow = false;
	bCanTriggerWarning = true;
}
