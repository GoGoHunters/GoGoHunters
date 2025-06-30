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


private:
	UPROPERTY() 
	UU_WebSocketManager* WebSocketManager;
};
