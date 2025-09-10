#include "LHJ/Trigger/CWorkingAreaTrigger.h"

#include "EngineUtils.h"
#include "Components/BoxComponent.h"
#include "JMH/MH_VRPlayer.h"
#include "Utilities/CHelpers.h"

ACWorkingAreaTrigger::ACWorkingAreaTrigger()
{
	PrimaryActorTick.bCanEverTick = true;

	CHelpers::CreateComponent<UBoxComponent>(this, &BoxComponent, "BoxComponent");
	CHelpers::CreateComponent<USceneComponent>(this, &AiLocSceneComponent, "AiLocSceneComponent", RootComponent);
}

void ACWorkingAreaTrigger::BeginPlay()
{
	Super::BeginPlay();

	if (bActiveTrigger)
	{
		BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ACWorkingAreaTrigger::OnTriggerBeginOverrlap);
		BoxComponent->OnComponentEndOverlap.AddDynamic(this, &ACWorkingAreaTrigger::OnTriggerEndOverrlap);

		// Pawn 중에 Tami 태그 찾아서 저장
		for (TActorIterator<APawn> It(GetWorld(), APawn::StaticClass()); It; ++It)
		{
			if (IsValid(*It) && (*It)->ActorHasTag(FName("Tami")))
			{
				TamiAI = *It;
				break;
			}
		}
	}
}

void ACWorkingAreaTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Player && !bTemporaryDeactivation)
	{
		FName FunctionName(TEXT("NotifyPlayerInTrigger"));
		UFunction* Function = TamiAI->FindFunction(FunctionName);
		if (Function)
		{
			FCNotifyPlayerInTrigger param;
			param.bPlayerInTrigger = true;
			param.TamiLocation = GetSceneCompLocation();
			TamiAI->ProcessEvent(Function, &param);
		}
		Player = nullptr;
	}
}

void ACWorkingAreaTrigger::OnTriggerBeginOverrlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                                  const FHitResult& SweepResult)
{
	// 플레이어가 아니면 패스
	if (!OtherActor->IsA(AMH_VRPlayer::StaticClass())) return;

	// 타미 없으면 패스
	if (!TamiAI) return;

	// 임시 비활성화 상태면 패스
	if (bTemporaryDeactivation)
	{
		Player = OtherActor;
	}
	else
	{
		FName FunctionName(TEXT("NotifyPlayerInTrigger"));
		UFunction* Function = TamiAI->FindFunction(FunctionName);
		if (Function)
		{
			FCNotifyPlayerInTrigger param;
			param.bPlayerInTrigger = true;
			param.TamiLocation = GetSceneCompLocation();
			TamiAI->ProcessEvent(Function, &param);
		}
	}	
}

void ACWorkingAreaTrigger::OnTriggerEndOverrlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// 플레이어가 아니면 패스
	if (!OtherActor->IsA(AMH_VRPlayer::StaticClass())) return;

	// 타미 없으면 패스
	if (!TamiAI) return;
	
	FName FunctionName(TEXT("NotifyPlayerInTrigger"));
	UFunction* Function = TamiAI->FindFunction(FunctionName);
	if (Function)
	{
		FCNotifyPlayerInTrigger param;
		param.bPlayerInTrigger = false;
		TamiAI->ProcessEvent(Function, &param);
	}
	Player = nullptr;
}

FVector ACWorkingAreaTrigger::GetSceneCompLocation()
{
	return AiLocSceneComponent->GetComponentLocation();
}
