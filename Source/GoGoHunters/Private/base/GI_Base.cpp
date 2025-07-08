// Fill out your copyright notice in the Description page of Project Settings.


#include "base/GI_Base.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

void UGI_Base::Init()
{
    Super::Init();

}

void UGI_Base::Shutdown()
{
    Super::Shutdown();
}

void UGI_Base::TransitionToLevel(const FString& TargetLevelName)
{
	// 이미 로딩 중이면 무시
	if (bIsLoadingLevel)
	{
		UE_LOG(LogTemp, Warning, TEXT("Already loading a level, ignoring transition request"));
		return;
	}
	
	// 타겟 레벨 정보 저장
	PendingTargetLevel = TargetLevelName;
	bIsLoadingLevel = true;
	
	// Transition Level로 이동
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("LV_MH_Loading"));
	
	UE_LOG(LogTemp, Log, TEXT("Transitioning to level: %s"), *TargetLevelName);
}

void UGI_Base::LoadTargetLevelAsync(const FString& TargetLevelName)
{
	// 비동기 로딩 시작
	FLatentActionInfo LatentInfo;
	LatentInfo.CallbackTarget = this;
	LatentInfo.ExecutionFunction = "OnLevelLoadComplete";
	LatentInfo.Linkage = 0;
	LatentInfo.UUID = 1;
	
	UGameplayStatics::LoadStreamLevel(GetWorld(), FName(*TargetLevelName), true, true, LatentInfo);
}

void UGI_Base::StartAsyncLoading()
{
	// Transition Level에서 호출되어 비동기 로딩 시작
	if (!PendingTargetLevel.IsEmpty() && bIsLoadingLevel)
	{
		LoadTargetLevelAsync(PendingTargetLevel);
		UE_LOG(LogTemp, Log, TEXT("Starting async loading for level: %s"), *PendingTargetLevel);
	}
}

void UGI_Base::OnLevelLoadComplete()
{
	// 로딩 완료 후 실제 레벨로 이동
	if (!PendingTargetLevel.IsEmpty())
	{
		FString TargetLevel = PendingTargetLevel; // 복사본 생성
		PendingTargetLevel.Empty();
		bIsLoadingLevel = false;
		
		UGameplayStatics::OpenLevel(GetWorld(), FName(*TargetLevel));
		
		UE_LOG(LogTemp, Log, TEXT("Level loading completed, transitioning to: %s"), *TargetLevel);
	}
}
