// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "IWebSocket.h"
#include "U_WebSocketManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWebSocketMessageReceived, const FString&, Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWebSocketConnectionStatusChanged, bool, bIsConnected);

/**
 * 
 */
UCLASS(Blueprintable)
class GOGOHUNTERS_API UU_WebSocketManager : public UObject
{
	GENERATED_BODY()

private:

    TSharedPtr<IWebSocket> WebSocket;

    // 웹소켓 이벤트 핸들러
    void OnWebSocketConnected();
    void OnWebSocketConnectionError(const FString& Error);
    void OnWebSocketClosed(int32 StatusCode, const FString& Reason, bool bWasClean);
    void OnWebSocketMessage(const FString& Message);

public:
    UU_WebSocketManager();

    UFUNCTION(BlueprintCallable, Category = "WebSocket")
    void WebSocketConnect(const FString& URL);

    UFUNCTION(BlueprintCallable, Category = "WebSocket")
    void WebSocketDisconnect();

    UFUNCTION(BlueprintCallable, Category = "WebSocket")
    void WebSocketSendMessage(const FString& Message);

    UFUNCTION(BlueprintPure, Category = "Networking|WebSocket")
    bool IsConnected() const;

    UPROPERTY(BlueprintAssignable, Category = "WebSocket")
    FOnWebSocketMessageReceived OnMessageReceived;

    UPROPERTY(BlueprintAssignable, Category = "WebSocket")
    FOnWebSocketConnectionStatusChanged OnConnectionStatusChanged;

};
