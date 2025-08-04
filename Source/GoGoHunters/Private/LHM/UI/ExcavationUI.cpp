// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/UI/ExcavationUI.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "Components/Button.h"
#include "JMH/MH_VRPlayer.h"
#include "LHM/Excavation/ExcavationManager.h"

void UExcavationUI::NativeConstruct()
{
	Super::NativeConstruct();

	VRPlayer = Cast<AMH_VRPlayer>(UGameplayStatics::GetPlayerCharacter(this, 0));

	// ExcavationManager 찾기 및 연결
	FindAndConnectExcavationManager();

	if (Btn_Tool1)Btn_Tool1->OnClicked.AddDynamic(this, &UExcavationUI::OnClick_DetectionTool);
	if (Btn_Tool2)Btn_Tool2->OnClicked.AddDynamic(this, &UExcavationUI::OnClick_ShovelTool);
	
	if (Btn_Tool3)Btn_Tool3->OnClicked.AddDynamic(this, &UExcavationUI::OnClick_BrushTool);
	
	if (Btn_Tool4)Btn_Tool4->OnClicked.AddDynamic(this, &UExcavationUI::OnClick_TweezerTool);
	
	// UI Tool 버튼 초기화
	if (ExcavationManager)
	{
		OnExcavationPhaseChanged(ExcavationManager->GetCurrentPhase());
	}
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
			TargetButton->SetRenderOpacity(0.7f);
		}
	}
}

void UExcavationUI::OnClick_DetectionTool()
{
	UE_LOG(LogTemp, Warning, TEXT("[ExcavationUI] OnClick_DetectionTool"));

	FString CurrentLevel = GetWorld()->GetMapName();
	CurrentLevel.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);
	if (CurrentLevel == "LV_TestExcavation")
	{
		if (ExcavationManager && ExcavationManager->IsToolAvailableForPhase(0))
		{
			if (VRPlayer) VRPlayer->ExcavationTool1();
		}
	}
	else
	{
		if (VRPlayer) VRPlayer->ExcavationTool1();
	}
}

void UExcavationUI::OnClick_ShovelTool()
{
	UE_LOG(LogTemp, Warning, TEXT("[ExcavationUI] OnClick_ShovelTool"));

	FString CurrentLevel = GetWorld()->GetMapName();
	CurrentLevel.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);
	if (CurrentLevel == "LV_TestExcavation")
	{
		if (ExcavationManager && ExcavationManager->IsToolAvailableForPhase(1))
		{
			if (VRPlayer) VRPlayer->ExcavationTool2();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[ExcavationUI] 현재 단계에서 삽 도구를 사용할 수 없습니다."));
		}
	}
	else
	{
		if (VRPlayer) VRPlayer->ExcavationTool2();
	}
}

void UExcavationUI::OnClick_BrushTool()
{
	UE_LOG(LogTemp, Warning, TEXT("[ExcavationUI] OnClick_BrushTool"));

	FString CurrentLevel = GetWorld()->GetMapName();
	CurrentLevel.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);
	if (CurrentLevel == "LV_TestExcavation")
	{
		if (ExcavationManager && ExcavationManager->IsToolAvailableForPhase(2))
		{
			if (VRPlayer) VRPlayer->ExcavationTool3();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[ExcavationUI] 현재 단계에서 붓 도구를 사용할 수 없습니다."));
		}
	}
	else
	{
		if (VRPlayer) VRPlayer->ExcavationTool3();
	}
}

void UExcavationUI::OnClick_TweezerTool()
{
	UE_LOG(LogTemp, Warning, TEXT("[ExcavationUI] OnClick_TweezerTool"));

	FString CurrentLevel = GetWorld()->GetMapName();
	CurrentLevel.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);
	if (CurrentLevel == "LV_TestExcavation")
	{
		if (ExcavationManager && ExcavationManager->IsToolAvailableForPhase(3))
		{
			if (VRPlayer) VRPlayer->ExcavationTool4();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[ExcavationUI] 현재 단계에서 집게 도구를 사용할 수 없습니다."));
		}
	}
	else
	{
		if (VRPlayer) VRPlayer->ExcavationTool4();
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
