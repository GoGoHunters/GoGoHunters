#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "WebSocket/U_WebSocketManager.h"
#include "LHJ/CRelicData.h"
#include "GameFramework/SaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "GI_Base.generated.h"

USTRUCT(BlueprintType)
struct FRelicSaveData
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite)
	FCRelicData RelicData;
};

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
	const TArray<FCRelicData>& GetAllRelicData() const { return RelicDataArray; }
	const FCRelicDetailData* GetRelicDetailDataByTag(const int32& RelicTag) const;

	UFUNCTION(BlueprintCallable)
	void SaveRelicData(FRelicSaveData NewData);
	UFUNCTION(BlueprintCallable)
	void SaveRelicCollectingData(FCRelicCollectingBook NewData);

private:
	UPROPERTY() 
	UU_WebSocketManager* WebSocketManager;
	
	// 레벨 전환 관련 변수들
	UPROPERTY()
	FString PendingTargetLevel;
	
	UPROPERTY()
	bool bIsLoadingLevel = false;

	// 유물 데이터
	UPROPERTY(EditDefaultsOnly, Category=DataTable)
	TObjectPtr<UDataTable> RelicDetailDataTable;
	UPROPERTY()
	TArray<FCRelicData> RelicDataArray;
	UPROPERTY()
	TMap<int32, FCRelicDetailData> RelicDetailDataMap;
	// 유물 데이터 관련
	void InitRelicDataFromSave();
	void InitRelicDetailData();

	UPROPERTY()
	TMap<int32, FCRelicCollectingBook> RelicCollectingBookMap;
	void InitRelicCollectingDataFromSave();
	void UpdateRelicCollectingData();
	const FCRelicCollectingBook* GetRelicCollectingDataByTag(const int32& RelicTag);
};

UCLASS()
class URelicSaveGame : public USaveGame
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite)
    TArray<FRelicSaveData> RelicSaveArray;
};

UCLASS()
class URelicCollectingBookSaveGame : public USaveGame
{
	GENERATED_BODY()
public:
	UPROPERTY()
	TArray<FCRelicCollectingBook> RelicCollectingBooks;
};
