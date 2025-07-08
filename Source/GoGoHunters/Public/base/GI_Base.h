// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "WebSocket/U_WebSocketManager.h"
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


private:
	UPROPERTY() 
	UU_WebSocketManager* WebSocketManager;
	
	// 레벨 전환 관련 변수들
	UPROPERTY()
	FString PendingTargetLevel;
	
	UPROPERTY()
	bool bIsLoadingLevel = false;
};
