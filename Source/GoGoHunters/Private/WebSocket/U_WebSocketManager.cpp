// Fill out your copyright notice in the Description page of Project Settings.


#include "WebSocket/U_WebSocketManager.h"
#include "WebSocketsModule.h"


// For Send File Event
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Misc/Guid.h"
#include "Serialization/BufferArchive.h"
#include "Containers/StringConv.h"

#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"


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


void UU_WebSocketManager::WebSocketSendFile(const FString& SaveFilePath, const FString& URL)
{
    FString FilePath = FPaths::ProjectSavedDir() / SaveFilePath;

    TArray<uint8> FileData;
    // 파일이 존재하는지, 읽을 수 있는지 확인

    if (!FFileHelper::LoadFileToArray(FileData, *FilePath))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load file: %s"), *FilePath);
        return;
    }

    // 파일 이름 추출
    FString FileName = FPaths::GetCleanFilename(FilePath);
    if (FileName.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid file name from path: %s"), *FilePath);
        return;
    }

    // HTTP 요청 생성
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(URL);
    Request->SetVerb(TEXT("POST"));
    // Content-Type을 multipart/form-data로 설정
    Request->SetHeader(TEXT("Content-Type"), TEXT("multipart/form-data; boundary=----------WebKitFormBoundaryABC123DEF456")); // 고유한 boundary 필요

    // 요청 바디 생성 (멀티파트 폼 데이터)
    FString Boundary = TEXT("----------WebKitFormBoundaryABC123DEF456");
    FString LineBreak = TEXT("\r\n");

    FString ContentDisposition = FString::Printf(TEXT("Content-Disposition: form-data; name=\"files\"; filename=\"%s\""), *FileName);
    FString ContentType = TEXT("Content-Type: application/octet-stream");

    // 바디 데이터 준비
    TArray<uint8> BodyData;

    // 시작 바운더리
    BodyData.Append((uint8*)TCHAR_TO_UTF8(*(LineBreak + "--" + Boundary + LineBreak + ContentDisposition + LineBreak + ContentType + LineBreak + LineBreak)),
        FCString::Strlen(*(LineBreak + "--" + Boundary + LineBreak + ContentDisposition + LineBreak + ContentType + LineBreak + LineBreak)));

    // 파일 데이터 추가
    BodyData.Append(FileData);

    // 끝 바운더리
    BodyData.Append((uint8*)TCHAR_TO_UTF8(*(LineBreak + "--" + Boundary + "--" + LineBreak)),
        FCString::Strlen(*(LineBreak + "--" + Boundary + "--" + LineBreak)));

    Request->SetContent(BodyData);

    // 응답 콜백 설정
    Request->OnProcessRequestComplete().BindUObject(this, &UU_WebSocketManager::OnFileUploadResponse);

    // 요청 전송
    Request->ProcessRequest();

    UE_LOG(LogTemp, Log, TEXT("Sending file '%s' to %s"), *FileName, *URL);

}

void UU_WebSocketManager::OnFileUploadResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (bWasSuccessful && Response.IsValid())
    {
        UE_LOG(LogTemp, Log, TEXT("File upload successful! Response: %s"), *Response->GetContentAsString());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("File upload failed. Status Code: %d, Error"), Response.IsValid() ? Response->GetResponseCode() : -1);
    }
}
