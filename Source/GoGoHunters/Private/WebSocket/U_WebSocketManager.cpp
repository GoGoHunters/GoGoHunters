// Fill out your copyright notice in the Description page of Project Settings.


#include "WebSocket/U_WebSocketManager.h"
#include "WebSocketsModule.h"

UU_WebSocketManager::UU_WebSocketManager()
{
    if (!FModuleManager::Get().IsModuleLoaded("WebSockets"))
    {
        FModuleManager::Get().LoadModule("WebSockets");
    }
}

void UU_WebSocketManager::WebSocketConnect(const FString& URL)
{
    if (WebSocket.IsValid() && WebSocket->IsConnected())
    {
        UE_LOG(LogTemp, Warning, TEXT("WebSocket already connected. Disconnecting first."));
        WebSocketDisconnect();
    }

    WebSocket = FWebSocketsModule::Get().CreateWebSocket(URL);

    WebSocket->OnConnected().AddUObject(this, &UU_WebSocketManager::OnWebSocketConnected);
    WebSocket->OnConnectionError().AddUObject(this, &UU_WebSocketManager::OnWebSocketConnectionError);
    WebSocket->OnClosed().AddUObject(this, &UU_WebSocketManager::OnWebSocketClosed);
    WebSocket->OnMessage().AddUObject(this, &UU_WebSocketManager::OnWebSocketMessage);

    WebSocket->Connect();
    UE_LOG(LogTemp, Log, TEXT("connect to WebSocket: %s"), *URL);
}

void UU_WebSocketManager::WebSocketDisconnect()
{
    if (WebSocket.IsValid() && WebSocket->IsConnected())
    {
        WebSocket->Close();
        UE_LOG(LogTemp, Log, TEXT("WebSocket disconnect initiated."));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("WebSocket is not valid or not connected, no disconnect needed."));
    }
    WebSocket.Reset();
    OnConnectionStatusChanged.Broadcast(false);
}

void UU_WebSocketManager::WebSocketSendMessage(const FString& Message)
{
    if (WebSocket.IsValid() && WebSocket->IsConnected())
    {
        WebSocket->Send(Message);
        UE_LOG(LogTemp, Log, TEXT("Sent WebSocket message: %s"), *Message);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot send message: WebSocket is not connected or valid."));
    }
}

bool UU_WebSocketManager::IsConnected() const
{
    return WebSocket.IsValid() && WebSocket->IsConnected();
}

void UU_WebSocketManager::OnWebSocketConnected()
{
    UE_LOG(LogTemp, Log, TEXT("WebSocket Connected!"));
    OnConnectionStatusChanged.Broadcast(true);
}

void UU_WebSocketManager::OnWebSocketConnectionError(const FString& Error)
{
    UE_LOG(LogTemp, Error, TEXT("WebSocket Connection Error: %s"), *Error);
    OnConnectionStatusChanged.Broadcast(false);
}

void UU_WebSocketManager::OnWebSocketClosed(int32 StatusCode, const FString& Reason, bool bWasClean)
{
    UE_LOG(LogTemp, Log, TEXT("WebSocket Closed. Code: %d, Reason: %s, Clean: %d"), StatusCode, *Reason, bWasClean);
    OnConnectionStatusChanged.Broadcast(false);
    WebSocket.Reset();
}

void UU_WebSocketManager::OnWebSocketMessage(const FString& Message)
{
    UE_LOG(LogTemp, Log, TEXT("Received WebSocket message: %s"), *Message);
    OnMessageReceived.Broadcast(Message);
}
