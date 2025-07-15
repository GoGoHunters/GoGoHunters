// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "IWebSocket.h"

#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"

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
    FString Server_URL;

    // 웹소켓 이벤트 핸들러
    void OnWebSocketConnected();
    void OnWebSocketConnectionError(const FString& Error);
    void OnWebSocketClosed(int32 StatusCode, const FString& Reason, bool bWasClean);
    void OnWebSocketMessage(const FString& Message);

    void OnFileUploadResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

    void OnFileDownloadComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FString DownloadedFilePath);


    bool EnsureDirectoryForFile(const FString& FilePath);

    bool WaitForFileToBeReadable(const FString& FilePath, int32 MaxAttempts = 10, float DelayPerAttempt = 0.1f);

    
     FString CleanWebSocketURL(const FString& InURL);

public:
    UU_WebSocketManager();

    UFUNCTION(BlueprintCallable, Category = "WebSocket")
    void WebSocketConnect(const FString& URL);

    UFUNCTION(BlueprintCallable, Category = "WebSocket")
    void WebSocketDisconnect();

    UFUNCTION(BlueprintCallable, Category = "WebSocket")
    void WebSocketSendMessage(const FString& Message);

    UFUNCTION(BlueprintCallable, Category = "WebSocket")
    void WebSocketSendByteData(const TArray<uint8>& DataToSend);

    UFUNCTION(BlueprintPure, Category = "WebSocket")
    bool IsConnected() const;

    UPROPERTY(BlueprintAssignable, Category = "WebSocket")
    FOnWebSocketMessageReceived OnMessageReceived;

    UPROPERTY(BlueprintAssignable, Category = "WebSocket")
    FOnWebSocketConnectionStatusChanged OnConnectionStatusChanged;

    // Sending File 
    UFUNCTION(BlueprintCallable, Category = "WebSocket")
    void WebSocketSendFile(const FString& FilePath, const FString& URLPath);

    UFUNCTION(BlueprintCallable, Category = "WebSocket|FileDownload")
    void WebSocketDownloadFile(const FString& URL, const FString& SaveAsFileName);

};

USTRUCT(BlueprintType)
struct FAIAnalysisResult
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "AIAnalysis")
    int32 Type; // JSON의 "type": 1 에 해당

    UPROPERTY(BlueprintReadWrite, Category = "AIAnalysis")
    FString Filename;

    UPROPERTY(BlueprintReadWrite, Category = "AIAnalysis")
    FString DownloadURL;

    UPROPERTY(BlueprintReadWrite, Category = "AIAnalysis")
    FString Timestamp;
};
