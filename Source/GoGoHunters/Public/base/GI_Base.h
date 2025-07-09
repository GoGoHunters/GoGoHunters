// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "WebSocket/U_WebSocketManager.h"
#include "LHJ/CRelicData.h"
#include "GI_Base.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class GOGOHUNTERS_API UGI_Base : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;
	virtual void Shutdown() override;
	
	// 레벨 전환 관련 함수들
	UFUNCTION(BlueprintCallable, Category = "Level Transition")
	void TransitionToLevel(const FString& TargetLevelName);
	
	UFUNCTION(BlueprintCallable, Category = "Level Transition")
	void LoadTargetLevelAsync(const FString& TargetLevelName);
	
	UFUNCTION(BlueprintCallable, Category = "Level Transition")
	void OnLevelLoadComplete();
	
	UFUNCTION(BlueprintCallable, Category = "Level Transition")
	void StartAsyncLoading();

	// 유물 데이터 관련
	const TMap<int32, FCRelicData>& GetAllRelicData() const { return RelicDataMap; }
	const FCRelicData* GetRelicDataByIndex(const int32 RelicIndex) const;
	const FCRelicDetailData* GetRelicDetailDataByName(const FString& RelicName) const;

private:
	UPROPERTY() 
	UU_WebSocketManager* WebSocketManager;
	
	// 레벨 전환 관련 변수들
	UPROPERTY()
	FString PendingTargetLevel;
	
	UPROPERTY()
	bool bIsLoadingLevel = false;

	// 유물 데이터
	UPROPERTY()
	TObjectPtr<UDataTable> RelicDataTable;
	UPROPERTY()
	TObjectPtr<UDataTable> RelicDetailDataTable;
	UPROPERTY()
	TMap<int32, FCRelicData> RelicDataMap;
	UPROPERTY()
	TMap<FString, FCRelicDetailData> RelicDetailDataMap;
	// 유물 데이터 관련
	void InitRelicData();
	void InitRelicDetailData();
};
