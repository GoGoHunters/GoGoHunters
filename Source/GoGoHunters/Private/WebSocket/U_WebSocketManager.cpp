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

// For JSON
#include "JsonObjectConverter.h"

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

    Server_URL = CleanWebSocketURL(URL);
    FString WebSocketURL = TEXT("ws://") + Server_URL + TEXT("/ws");

    WebSocket = FWebSocketsModule::Get().CreateWebSocket(WebSocketURL);

    WebSocket->OnConnected().AddUObject(this, &UU_WebSocketManager::OnWebSocketConnected);
    WebSocket->OnConnectionError().AddUObject(this, &UU_WebSocketManager::OnWebSocketConnectionError);
    WebSocket->OnClosed().AddUObject(this, &UU_WebSocketManager::OnWebSocketClosed);
    WebSocket->OnMessage().AddUObject(this, &UU_WebSocketManager::OnWebSocketMessage);

    WebSocket->OnRawMessage().AddUObject(this, &UU_WebSocketManager::OnWebSocketRawMessage);

    WebSocket->Connect();
    UE_LOG(LogTemp, Log, TEXT("connect to WebSocket: %s"), *WebSocketURL);
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

void UU_WebSocketManager::WebSocketSendByteData(const TArray<uint8>& DataToSend)
{
    if (WebSocket.IsValid() && WebSocket->IsConnected())
    {
        WebSocket->Send(DataToSend.GetData(), DataToSend.Num(), true);
        UE_LOG(LogTemp, Warning, TEXT("Sent %d bytes to server."), DataToSend.Num());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("WebSocket not connected. Cannot send data."));
    }
}

void UU_WebSocketManager::SendEndOfDataSignal()
{
    if (WebSocket.IsValid() && WebSocket->IsConnected())
    {
        TArray<uint8> EndSignal;
        EndSignal.Add(0xFE);

        WebSocket->Send(EndSignal.GetData(), EndSignal.Num(), true);
        UE_LOG(LogTemp, Warning, TEXT("Sent EndOfDataSignal (0xFE) via WebSocket."));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("WebSocket not connected. Cannot send EndOfDataSignal."));
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
    // 추가적으로 재시도 함수 만들기 필요
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

    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Message);


    if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
    {
        UE_LOG(LogTemp, Log, TEXT("Successfully parsed WebSocket message as JSON."));
        
        if (JsonObject->HasField(TEXT("type")))
        {
            double MessageTypeNumber;

            if (JsonObject->TryGetNumberField(TEXT("type"), MessageTypeNumber))
            {
                if (FMath::IsNearlyEqual(MessageTypeNumber, 1.0))
                {
                    FAIAnalysisResult ParsedResult;
                    UE_LOG(LogTemp, Log, TEXT("Broadcasting AI analysis result (Type: 1)."));
                    if (FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), &ParsedResult, 0, 0))
                    {
                        WebSocketDownloadFile(ParsedResult.DownloadURL, ParsedResult.Filename);
                    }
                }
            }
        }

    }

}

void UU_WebSocketManager::OnWebSocketRawMessage(const void* Data, SIZE_T Size, SIZE_T BytesRemaining)
{
    UE_LOG(LogTemp, Log, TEXT("Received WebSocket raw (byte) message. Size: %lld, Bytes Remaining: %lld"), Size, BytesRemaining);

    CurrentIncomingMessageBuffer.Append((const uint8*)Data, Size);

    if (BytesRemaining > 0)
    {
        UE_LOG(LogTemp, Log, TEXT("  -- More data for this message is expected. Accumulating... Total accumulated: %d bytes"), CurrentIncomingMessageBuffer.Num());
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("  -- Full message received. Total size: %d bytes. Broadcasting now."), CurrentIncomingMessageBuffer.Num());

        OnByteDataReceived.Broadcast(CurrentIncomingMessageBuffer);

        // 문제가 있을거 같음 버퍼가 언제 지워지는가?
        CurrentIncomingMessageBuffer.Empty();
    }

}

void UU_WebSocketManager::WebSocketSendFile(const FString& FilePath, const FString& URLPath)
{
    FString FullFilePath = FilePath;
    FString FileName = FPaths::GetCleanFilename(FullFilePath);

    FString WebSendURL = TEXT("http://") + Server_URL / URLPath / "";

    TArray<uint8> FileData;

    if (!WaitForFileToBeReadable(FullFilePath))
    {
        UE_LOG(LogTemp, Error, TEXT("WebSocketSendFile: File '%s' is not readable after multiple attempts. Aborting upload."), *FullFilePath);
        return;
    }

    if (!FFileHelper::LoadFileToArray(FileData, *FullFilePath))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load file: %s"), *FullFilePath);
        return;
    }

    if (FileName.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid file name from path: %s"), *FullFilePath);
        return;
    }
    
    // HTTP 요청 생성
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(WebSendURL);
    Request->SetVerb(TEXT("POST"));
    // Content-Type을 multipart/form-data로 설정
    Request->SetHeader(TEXT("Content-Type"), TEXT("multipart/form-data; boundary=----------WebKitFormBoundaryABC123DEF456")); // 고유한 boundary 필요

    // 요청 바디 생성 (멀티파트 폼 데이터)
    // FString Boundary = FString::Printf(TEXT("----------Boundary%s"), *FGuid::NewGuid().ToString().Replace(TEXT("-"), TEXT(""))); 생성식  boundary 추후 수정
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

    UE_LOG(LogTemp, Log, TEXT("Sending file '%s' to %s"), *FileName, *WebSendURL);

}

void UU_WebSocketManager::OnFileUploadResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (bWasSuccessful && Response.IsValid())
    {
        UE_LOG(LogTemp, Log, TEXT("File upload successful! Response: %s"), *Response->GetContentAsString());
        FString Message = Response->GetContentAsString();
        OnMessageReceived.Broadcast(Message);

        TSharedPtr<FJsonObject> JsonObject;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Message);


        if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
        {
            UE_LOG(LogTemp, Log, TEXT("Successfully parsed WebSocket message as JSON."));

            if (JsonObject->HasField(TEXT("type")))
            {
                double MessageTypeNumber;

                if (JsonObject->TryGetNumberField(TEXT("type"), MessageTypeNumber))
                {
                    if (FMath::IsNearlyEqual(MessageTypeNumber, 1.0))
                    {
                        FAIAnalysisResult ParsedResult;
                        UE_LOG(LogTemp, Log, TEXT("Broadcasting AI analysis result (Type: 1)."));
                        if (FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), &ParsedResult, 0, 0))
                        {
                            WebSocketDownloadFile(ParsedResult.DownloadURL, ParsedResult.Filename);
                        }
                    }
                }
            }

        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("File upload failed. Status Code: %d, Error"), Response.IsValid() ? Response->GetResponseCode() : -1);
    }
}


void UU_WebSocketManager::WebSocketDownloadFile(const FString& URL, const FString& SaveAsFileName)
{
    if (URL.IsEmpty() || SaveAsFileName.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("Download Param Error empty param."));
        return;
    }

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(URL);
    Request->SetVerb(TEXT("GET"));

    FString FullSavePath = FPaths::ProjectSavedDir() / "Answer" / SaveAsFileName;
    
    if (!EnsureDirectoryForFile(FullSavePath))
        return;

    Request->OnProcessRequestComplete().BindUObject(this, &UU_WebSocketManager::OnFileDownloadComplete, FullSavePath);

    Request->ProcessRequest();
    UE_LOG(LogTemp, Log, TEXT("file download from '%s' to '%s'"), *URL, *FullSavePath);
}

void UU_WebSocketManager::WebSocketSendByteFile(const FString& FilePath)
{
    FString FullFilePath = FilePath;
    FString FileName = FPaths::GetCleanFilename(FullFilePath);

    TArray<uint8> FileData;

    if (!WaitForFileToBeReadable(FullFilePath))
    {
        UE_LOG(LogTemp, Error, TEXT("WebSocketSendFile: File '%s' is not readable after multiple attempts. Aborting upload."), *FullFilePath);
        return;
    }

    if (!FFileHelper::LoadFileToArray(FileData, *FullFilePath))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load file: %s"), *FullFilePath);
        return;
    }

    if (FileName.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid file name from path: %s"), *FullFilePath);
        return;
    }
    WebSocketSendByteData(FileData);
}

void UU_WebSocketManager::OnFileDownloadComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FString DownloadedFilePath)
{
    if (bWasSuccessful && Response.IsValid())
    {
        const TArray<uint8>& FileData = Response->GetContent();

            if (FFileHelper::SaveArrayToFile(FileData, *DownloadedFilePath))
            {
                UE_LOG(LogTemp, Log, TEXT("File downloaded successfully to: %s"), *DownloadedFilePath);
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to save downloaded file to: %s"), *DownloadedFilePath);
            }
    }
    else
    {
        int32 StatusCode = Response.IsValid() ? Response->GetResponseCode() : -1;

        UE_LOG(LogTemp, Error, TEXT("File download failed from URL: %s. Status Code: %d"),
            *Request->GetURL(), StatusCode);
    }
}

bool UU_WebSocketManager::EnsureDirectoryForFile(const FString& FilePath)
{
    FString SaveDirectory = FPaths::GetPath(FilePath);

    if (!IFileManager::Get().DirectoryExists(*SaveDirectory))
    {
        if (!IFileManager::Get().MakeDirectory(*SaveDirectory, true))
        {
            UE_LOG(LogTemp, Error, TEXT("EnsureDirectoryForFile: Failed to create directory: %s"), *SaveDirectory);
            return false;
        }
        else
        {
            UE_LOG(LogTemp, Log, TEXT("EnsureDirectoryForFile: Created directory: %s"), *SaveDirectory);
        }
    }
    return true; 
}

bool UU_WebSocketManager::WaitForFileToBeReadable(const FString& FilePath, int32 MaxAttempts, float DelayPerAttempt)
{
    IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

    for (int32 Attempt = 0; Attempt < MaxAttempts; ++Attempt)
    {
        if (!PlatformFile.FileExists(*FilePath))
        {
            UE_LOG(LogTemp, Warning, TEXT("WaitForFileToBeReadable: File '%s' does not exist yet. Attempt %d/%d."), *FilePath, Attempt + 1, MaxAttempts);
            FPlatformProcess::Sleep(DelayPerAttempt);
            continue;
        }
        IFileHandle* FileHandle = PlatformFile.OpenRead(*FilePath);
        if (FileHandle)
        {
            delete FileHandle;
            UE_LOG(LogTemp, Log, TEXT("WaitForFileToBeReadable: File '%s' is now readable after %d attempts."), *FilePath, Attempt + 1);
            return true;
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("WaitForFileToBeReadable: File '%s' is not readable. Attempt %d/%d. Retrying in %.2f seconds."), *FilePath, Attempt + 1, MaxAttempts, DelayPerAttempt);
            FPlatformProcess::Sleep(DelayPerAttempt);
        }
    }

    UE_LOG(LogTemp, Error, TEXT("WaitForFileToBeReadable: File '%s' failed to become readable after %d attempts."), *FilePath, MaxAttempts);
    return false;
}

FString UU_WebSocketManager::CleanWebSocketURL(const FString& InURL)
{

    FString CleanedURL = InURL;

    CleanedURL.TrimStartAndEndInline();

    if (CleanedURL.StartsWith(TEXT("ws://")))
        CleanedURL = CleanedURL.Right(CleanedURL.Len() - 5);
    else if (CleanedURL.StartsWith(TEXT("wss://")))
        CleanedURL = CleanedURL.Right(CleanedURL.Len() - 6);
    else if (CleanedURL.StartsWith(TEXT("http://")))
        CleanedURL = CleanedURL.Right(CleanedURL.Len() - 7); 
    else if (CleanedURL.StartsWith(TEXT("https://")))
        CleanedURL = CleanedURL.Right(CleanedURL.Len() - 8);

    int32 FirstSlashIndex;
    if (CleanedURL.FindChar(TEXT('/'), FirstSlashIndex))
        CleanedURL = CleanedURL.Left(FirstSlashIndex);
    int32 FirstQuestionMarkIndex;
    if (CleanedURL.FindChar(TEXT('?'), FirstQuestionMarkIndex))
        CleanedURL = CleanedURL.Left(FirstQuestionMarkIndex);

    return CleanedURL;
}



