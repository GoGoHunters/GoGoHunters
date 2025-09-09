// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/Excavation/ExcavationManager.h"
#include "LHM/Excavation/RelicsManager.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "JMH/CMuseumComponent.h"
#include "base/GI_Base.h"
#include "LHM/Excavation/RelicsBase.h"
#include "LHM/UI/DiggingUI.h"
#include "LHM/UI/BrushingUI.h"
#include "UIs/CUiActor.h"
#include "LHM/UI/ExcavationPhaseUI.h"
#include "LHM/Excavation/CollectionBox.h"
#include "LHJ/Tutorial/CTutorialManager.h"
#include "LHM/UI/WarningUI.h"

// Sets default values
AExcavationManager::AExcavationManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 기본 단계별 도구 가용성 설정
	ToolAvailabilityByPhase.SetNum(4); // 4개 도구
	ToolAvailabilityByPhase[0] = true;  // 탐지 도구 - 항상 사용 가능
	ToolAvailabilityByPhase[1] = false; // 삽 도구 - 삽질 단계에서만
	ToolAvailabilityByPhase[2] = false; // 붓 도구 - 붓질 단계에서만
	ToolAvailabilityByPhase[3] = false; // 집게 도구 - 수거 단계에서만
}

// Called when the game starts or when spawned
void AExcavationManager::BeginPlay()
{
	Super::BeginPlay();
	
	for (TActorIterator<ARelicsManager> It(GetWorld()); It; ++It)
	{
		AllRelicsManagers.Add(*It);
	}

	// 초기 단계 설정
	SetCurrentPhase(EExcavationPhase::Detection);
}

void AExcavationManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(LobbyMuseumTimerHandle);
	GetWorldTimerManager().ClearAllTimersForObject(this);

	Super::EndPlay(EndPlayReason);
}

// Called every frame
void AExcavationManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    // Digging 단계일 때만 진행률 UI 업데이트
    if (CurrentPhase == EExcavationPhase::Digging)
    {
		UpdateDiggingProgress();
    }
}

void AExcavationManager::NotifyDetectionCompleted(class ARelicsManager* FromManager)
{
	if (!IsValid(FromManager)) return;
	if (!PhaseUI) return;

	CurrentActiveManager = FromManager;

	// 타미 음성
	PlayTami(TEXT("PlayExcavationPhase2_StartFlag"));

	// Phase UI 가시화 (깃발 트리거)
	// Phase UI에서 완료버튼 클릭하면 발굴 단계로 전환
	FTimerHandle PhaseUIVisibilityHandle;
	GetWorldTimerManager().SetTimer(PhaseUIVisibilityHandle, [this]()
	{
		if (PhaseUI->bUseFlagTrigger) return; // 이미 가시화된 경우 중복 실행 방지
		PhaseUI->SetVisibilityFlagTrigger(true);
	}, 9.0f, false);
}

void AExcavationManager::NotifyExcavationCompleted(class ARelicsManager* FromManager)
{
	if (!IsValid(FromManager)) return;
	if (!DiggingUI || !BrushingUI) return;

	// 타미 음성
	PlayTami(TEXT("PlayExcavationPhase4_DiscoveryRelic"));

	DiggingUI->SetVisibility(ESlateVisibility::Hidden);
	BrushingUI->SetVisibility(ESlateVisibility::Visible);

	// 붓질 단계로 전환
	SetCurrentPhase(EExcavationPhase::Brushing);
	UE_LOG(LogTemp, Log, TEXT("[ExcavationManager] 땅 파기 완료! 붓질 단계로 전환: %s"), *FromManager->GetName());
}

void AExcavationManager::NotifyDustingCompleted(class ARelicsManager* FromManager)
{
	if (!IsValid(FromManager)) return;
	if (!WarningUI) return;

	// 경고 UI 리셋
	WarningUI->ResetWarnings();

	// 수거박스 생성 요청
	FromManager->SpawnCollectionBox();

	// 타미 음성
	PlayTami(TEXT("PlayExcavationPhase5_StartCollection"));

	// 수거 단계로 전환
	SetCurrentPhase(EExcavationPhase::Collection);
	UE_LOG(LogTemp, Log, TEXT("[ExcavationManager] 붓질 완료! 수거 단계로 전환: %s"), *FromManager->GetName());
}

void AExcavationManager::NotifyCollectionCompleted(class ARelicsManager* FromManager, class ACollectionBox* FromCollectionBox)
{
	if (!IsValid(FromManager)) return;
	if (!IsValid(FromCollectionBox)) return;
	if (!PhaseUI || !WarningUI) return;

	CurrentActiveManager = FromManager;
	CollectionBox = FromCollectionBox;

	PlayTami(TEXT("PlayExcavationCompleted1"));

	// Phase UI 가시화 (수거함 완료 트리거)
	// Phase UI에서 완료버튼 클릭하면 수거함 닫기
	PhaseUI->SetVisibilityCloseLid(true);

	// 경고 UI 리셋
	WarningUI->ResetWarnings();
}

void AExcavationManager::SetCurrentPhase(EExcavationPhase NewPhase)
{
	if (CurrentPhase == NewPhase) return;

	CurrentPhase = NewPhase;

	// 단계별 도구 가용성 업데이트
	switch (CurrentPhase)
	{
	case EExcavationPhase::Detection:
		ToolAvailabilityByPhase[0] = true;  // 탐지 도구
		ToolAvailabilityByPhase[1] = false; // 삽 도구
		ToolAvailabilityByPhase[2] = false; // 붓 도구
		ToolAvailabilityByPhase[3] = false; // 집게 도구
		break;

	case EExcavationPhase::Digging:
		ToolAvailabilityByPhase[0] = true;  // 탐지 도구 (여전히 사용 가능)
		ToolAvailabilityByPhase[1] = true;  // 삽 도구
		ToolAvailabilityByPhase[2] = false; // 붓 도구
		ToolAvailabilityByPhase[3] = false; // 집게 도구
		break;

	case EExcavationPhase::Brushing:
		ToolAvailabilityByPhase[0] = true;  // 탐지 도구
		ToolAvailabilityByPhase[1] = true;  // 삽 도구 (여전히 사용 가능)
		ToolAvailabilityByPhase[2] = true;  // 붓 도구
		ToolAvailabilityByPhase[3] = false; // 집게 도구
		break;

	case EExcavationPhase::Collection:
		ToolAvailabilityByPhase[0] = true;  // 탐지 도구
		ToolAvailabilityByPhase[1] = true;  // 삽 도구
		ToolAvailabilityByPhase[2] = true;  // 붓 도구
		ToolAvailabilityByPhase[3] = true;  // 집게 도구
		break;

	case EExcavationPhase::Completed:
		// 모든 도구 사용 가능 (완료 후 자유롭게 사용)
		ToolAvailabilityByPhase[0] = true;
		ToolAvailabilityByPhase[1] = true;
		ToolAvailabilityByPhase[2] = true;
		ToolAvailabilityByPhase[3] = true;
		break;
	}

	// Brushing 단계 진입 시점에 BrushingUI를 RelicsBase에 연결
	if (CurrentPhase == EExcavationPhase::Brushing)
	{
		if (BrushingUI && CurrentActiveManager && CurrentActiveManager->GetRelics())
		{
			CurrentActiveManager->GetRelics()->SetBrushingUI(BrushingUI);
			CurrentActiveManager->GetRelics()->SetWarningUI(WarningUI);
		}
	}

	OnExcavationPhaseChanged.Broadcast(CurrentPhase);
	UE_LOG(LogTemp, Log, TEXT("[ExcavationManager] 발굴 단계 변경: %d"), (int32)CurrentPhase);
}

bool AExcavationManager::IsToolAvailableForPhase(int32 ToolIndex) const
{
	if (ToolIndex >= 0 && ToolIndex < ToolAvailabilityByPhase.Num())
	{
		return ToolAvailabilityByPhase[ToolIndex];
	}
	return false;
}

void AExcavationManager::ChangeExcavationPhase()
{
	if( !CurrentActiveManager || !CurrentActiveManager->GetRelics() ) return;
	if (!PhaseUI || !DiggingUI) return;
	
	CurrentActiveManager->GetRelics()->ActivateMarker();

	CurrentActiveManager->StartExcavation();

	PlayTami(TEXT("PlayExcavationPhase2_PlantedFlag"));

	// Progress UI 가시화
	PhaseUI->SetVisibilityFlagTrigger(false);
	DiggingUI->SetVisibility(ESlateVisibility::Visible);
	//PlayPopupUiAnim(false);

	// 삽질 단계로 전환
	SetCurrentPhase(EExcavationPhase::Digging);
}

void AExcavationManager::ChangeCompletedPhase()
{
	if (!IsValid(CurrentActiveManager)) return;
	if (!IsValid(CollectionBox)) return;
	if (!BrushingUI) return;

	PhaseUI->SetVisibilityCloseLid(false);
	BrushingUI->SetVisibility(ESlateVisibility::Hidden);
	//PlayPopupUiAnim(true);

	// 수거함 닫기 애니메이션
	CollectionBox->PlayBoxCloseAnimation();

	// 타미 음성
	PlayTami(TEXT("PlayExcavationCompleted2"));

	GetWorldTimerManager().ClearTimer(KeyboardSpawnTimerHandle);

	FTimerDelegate D;
	D.BindUObject(this, &AExcavationManager::SpawnKeyboardActor);
	GetWorldTimerManager().SetTimer(KeyboardSpawnTimerHandle, D, 8.f, false);

	/*// Phase UI (로비/박물관 이동)
	
	//FTimerHandle PhaseUIVisibilityHandle;
	//GetWorldTimerManager().SetTimer(PhaseUIVisibilityHandle, [this]()
	//{
	//	if(bUseBtnLobbynMuseum) return;
	//	PhaseUI->SetVisibilityLobby(true);
	//	PhaseUI->SetVisibilityMuseum(true);
	//}, 15.0f, false);
	
	// 기존 람다 타이머 제거 → 안전한 바인딩 사용
	GetWorldTimerManager().ClearTimer(LobbyMuseumTimerHandle);

	// BindUObject 사용: UObject 생명주기와 함께 안전해짐
	FTimerDelegate D;
	D.BindUObject(this, &AExcavationManager::ShowLobbyMuseumButtons);
	GetWorldTimerManager().SetTimer(LobbyMuseumTimerHandle, D, 15.0f, false);*/

	// 게임 인스턴스에서 유물 등록
	if (UGI_Base* GI = Cast<UGI_Base>(UGameplayStatics::GetGameInstance(GetWorld())))
	{
		// 플레이어의 MuseumComponent를 찾아서 RegisterRelic 호출
		if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
		{
			if (APawn* Pawn = PC->GetPawn())
			{
				if (UCMuseumComponent* MuseumComponent = Pawn->FindComponentByClass<UCMuseumComponent>())
				{
					// RelicsBase에서 유물 태그 가져오기
					ARelicsBase* Relics = CurrentActiveManager->GetRelics();
					int32 RelicTag = -1;
					if (Relics)
					{
						RelicTag = Relics->GetRelicTag();
					}

					MuseumComponent->RegisterRelic(RelicTag);
					UE_LOG(LogTemp, Log, TEXT("[ExcavationManager] 유물 등록 완료 - 태그: %d"), RelicTag);
				}
			}
		}
	}

	// 발굴 완료 단계로 전환
	SetCurrentPhase(EExcavationPhase::Completed);
	UE_LOG(LogTemp, Log, TEXT("[ExcavationManager] 수거 완료! 발굴 완료: %s"), *CurrentActiveManager->GetName());
}

void AExcavationManager::PlayPopupUiAnim(bool IsTunrOff)
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACUiActor::StaticClass(), FoundActors);

	if (FoundActors.Num() > 0)
	{
		if (ACUiActor* UiActor = Cast<ACUiActor>(FoundActors[0]))
		{
			if (IsTunrOff) UiActor->K2_PlayPopupUiAnim(true);
			else UiActor->K2_PlayPopupUiAnim(false);
		}
	}
}

void AExcavationManager::UpdateDiggingProgress()
{
	if (!CurrentActiveManager) return;
	if (!DiggingUI) return;
	
	float DigProgress = 0.0f;
	if (CurrentActiveManager->GetCurrentDigProgress(DigProgress))
	{
		float ProgressPercent = FMath::Clamp(DigProgress, 0.0f, 100.0f);
		DiggingUI->UpdateUI(ProgressPercent);
	}
}

void AExcavationManager::HandleWarningReset()
{
	switch (CurrentPhase)
	{
	case EExcavationPhase::Brushing:
		if(CurrentActiveManager->GetRelics())
		{
			CurrentActiveManager->GetRelics()->ResetDecalsAndProgress();
		}
		break;

	case EExcavationPhase::Collection:
		if (CurrentActiveManager->GetCollectionBox())
		{
			CurrentActiveManager->GetCollectionBox()->ResetCollectedRelics();
		}
		break;

	default:
		break;
	}
}

void AExcavationManager::PlayTami(const FName& FunctionName)
{
	// 타미 음성
	for (TActorIterator<APawn> It(GetWorld(), APawn::StaticClass()); It; ++It)
	{
		if (IsValid(*It) && (*It)->ActorHasTag(FName("Tami")))
		{
			if (APawn* TamiAI = *It)
			{
				if (UFunction* Function = TamiAI->FindFunction(FunctionName))
				{
					TamiAI->ProcessEvent(Function, nullptr);
				}
			}
			break;
		}
	}
}

void AExcavationManager::SpawnKeyboardActor()
{
	if(!IsValid(CurrentActiveManager)) return;
	CurrentActiveManager->SpawnKeyboard();
}

void AExcavationManager::ShowLobbyMuseumButtons()
{
	if (!IsValid(this) || !IsValid(PhaseUI) || bUseBtnLobbynMuseum) return;
	PhaseUI->SetVisibilityLobby(true);
	PhaseUI->SetVisibilityMuseum(true);
}
