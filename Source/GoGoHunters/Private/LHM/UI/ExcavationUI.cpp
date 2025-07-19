// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/UI/ExcavationUI.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"
#include "JMH/MH_VRPlayer.h"
#include "LHM/Excavation/ExcavationManager.h"
#include "EngineUtils.h"

void UExcavationUI::NativeConstruct()
{
	VRPlayer = Cast<AMH_VRPlayer>(UGameplayStatics::GetPlayerCharacter(this, 0));
	
	// ExcavationManager 찾기 및 연결
	FindAndConnectExcavationManager();
	
	if (Btn_Tool1) Btn_Tool1->OnClicked.AddDynamic(this, &UExcavationUI::OnClick_DetectionTool);
	if (Btn_Tool2) Btn_Tool2->OnClicked.AddDynamic(this, &UExcavationUI::OnClick_ShovelTool);
	if (Btn_Tool3) Btn_Tool3->OnClicked.AddDynamic(this, &UExcavationUI::OnClick_BrushTool);
	if (Btn_Tool4) Btn_Tool4->OnClicked.AddDynamic(this, &UExcavationUI::OnClick_TweezerTool);

	// 초기 UI 업데이트
	UpdateToolAvailability();
}

void UExcavationUI::NativeDestruct()
{
	// 델리게이트 구독 해제
	if (ExcavationManager)
	{
		ExcavationManager->OnExcavationPhaseChanged.RemoveDynamic(this, &UExcavationUI::OnExcavationPhaseChanged);
	}

	Super::NativeDestruct();
}

void UExcavationUI::OnExcavationPhaseChanged(EExcavationPhase NewPhase)
{
	// 실시간으로 단계 변경 감지
	CurrentPhase = NewPhase;
	
	// 각 도구의 가용성 확인 및 버튼 상태 업데이트
	for (int32 i = 0; i < 4; ++i)
	{
		bool bIsAvailable = ExcavationManager->IsToolAvailableForPhase(i);
		SetToolButtonEnabled(i, bIsAvailable);
	}

	UE_LOG(LogTemp, Log, TEXT("[ExcavationUI] 델리게이트로 발굴 단계 변경 감지: %d"), (int32)CurrentPhase);
}

void UExcavationUI::UpdateToolAvailability()
{
	if (!ExcavationManager) return;

	// 현재 발굴 단계 가져오기
	EExcavationPhase NewPhase = ExcavationManager->GetCurrentPhase();
	
	// 단계가 변경되었을 때만 UI 업데이트
	if (CurrentPhase != NewPhase)
	{
		CurrentPhase = NewPhase;
		
		// 각 도구의 가용성 확인 및 버튼 상태 업데이트
		for (int32 i = 0; i < 4; ++i)
		{
			bool bIsAvailable = ExcavationManager->IsToolAvailableForPhase(i);
			SetToolButtonEnabled(i, bIsAvailable);
		}

		UE_LOG(LogTemp, Log, TEXT("[ExcavationUI] 발굴 단계 변경으로 UI 업데이트: %d"), (int32)CurrentPhase);
	}
}

void UExcavationUI::SetToolButtonEnabled(int32 ToolIndex, bool bEnabled)
{
	UButton* TargetButton = nullptr;
	
	switch (ToolIndex)
	{
	case 0:
		TargetButton = Btn_Tool1;
		break;
	case 1:
		TargetButton = Btn_Tool2;
		break;
	case 2:
		TargetButton = Btn_Tool3;
		break;
	case 3:
		TargetButton = Btn_Tool4;
		break;
	}

	if (TargetButton)
	{
		TargetButton->SetIsEnabled(bEnabled);
		
		// 시각적 피드백을 위한 투명도 조정
		if (bEnabled)
		{
			TargetButton->SetRenderOpacity(1.0f);
		}
		else
		{
			TargetButton->SetRenderOpacity(0.3f);
		}
	}
}

void UExcavationUI::OnClick_DetectionTool()
{
	if (!ExcavationManager) return;
	
	// 현재 단계에서 탐지 도구 사용 가능한지 확인
	if (ExcavationManager->IsToolAvailableForPhase(0))
	{
		if(VRPlayer) VRPlayer->ExcavationTool1();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[ExcavationUI] 현재 단계에서 탐지 도구를 사용할 수 없습니다."));
	}
}

void UExcavationUI::OnClick_ShovelTool()
{
	if (!ExcavationManager) return;
	
	// 현재 단계에서 삽 도구 사용 가능한지 확인
	if (ExcavationManager->IsToolAvailableForPhase(1))
	{
		if (VRPlayer) VRPlayer->ExcavationTool2();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[ExcavationUI] 현재 단계에서 삽 도구를 사용할 수 없습니다."));
	}
}

void UExcavationUI::OnClick_BrushTool()
{
	if (!ExcavationManager) return;
	
	// 현재 단계에서 붓 도구 사용 가능한지 확인
	if (ExcavationManager->IsToolAvailableForPhase(2))
	{
		if (VRPlayer) VRPlayer->ExcavationTool3();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[ExcavationUI] 현재 단계에서 붓 도구를 사용할 수 없습니다."));
	}
}

void UExcavationUI::OnClick_TweezerTool()
{
	if (!ExcavationManager) return;
	
	// 현재 단계에서 집게 도구 사용 가능한지 확인
	if (ExcavationManager->IsToolAvailableForPhase(3))
	{
		if (VRPlayer) VRPlayer->ExcavationTool4();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[ExcavationUI] 현재 단계에서 집게 도구를 사용할 수 없습니다."));
	}
}

void UExcavationUI::FindAndConnectExcavationManager()
{
	// ExcavationManager 찾기
	for (TActorIterator<AExcavationManager> It(GetWorld()); It; ++It)
	{
		ExcavationManager = *It;
		break;
	}
	
	// 델리게이트 구독
	if (ExcavationManager)
	{
		ExcavationManager->OnExcavationPhaseChanged.AddDynamic(this, &UExcavationUI::OnExcavationPhaseChanged);
		UE_LOG(LogTemp, Log, TEXT("[ExcavationUI] ExcavationManager 연결 완료"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[ExcavationUI] ExcavationManager를 찾을 수 없습니다"));
	}
}
