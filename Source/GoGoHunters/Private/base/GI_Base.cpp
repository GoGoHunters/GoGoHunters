#include "base/GI_Base.h"
#include "Engine/DataTable.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "LHJ/CRelicData.h"
#include "Engine/World.h"
#include "GameFramework/SaveGame.h"

void UGI_Base::Init()
{
    Super::Init();
    InitRelicDataFromSave();
	InitRelicDetailData();
}

void UGI_Base::InitRelicDataFromSave()
{
    RelicDataArray.Empty();

    if (UGameplayStatics::DoesSaveGameExist(TEXT("RelicSaveSlot"), 0))
    {
        URelicSaveGame* LoadedGame = Cast<URelicSaveGame>(UGameplayStatics::LoadGameFromSlot(TEXT("RelicSaveSlot"), 0));
        if (LoadedGame)
        {
            for (const FRelicSaveData& SaveData : LoadedGame->RelicSaveArray)
            {
                RelicDataArray.Add(SaveData.RelicData);
            }
        }
    }
}

void UGI_Base::InitRelicDetailData()
{
	if (!RelicDetailDataTable) return;

	RelicDetailDataMap.Empty();
	TArray<FName> RowNames = RelicDetailDataTable->GetRowNames();
	for (const FName& RowName : RowNames)
	{
		FCRelicDetailData* Row = RelicDetailDataTable->FindRow<FCRelicDetailData>(RowName, TEXT("InitRelicData"));
		if (Row)
		{
			RelicDetailDataMap.Add(Row->RelicName.ToString(), *Row);
		}
	}
}

const FCRelicDetailData* UGI_Base::GetRelicDetailDataByName(const FString& RelicName) const
{
	return RelicDetailDataMap.Find(RelicName);
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

void UGI_Base::SaveRelicData(FRelicSaveData NewData)
{
    // 1. RelicDataArray에서 동일한 Date를 가진 데이터가 있는지 찾기
    bool bFound = false;
    for (FCRelicData& Data : RelicDataArray)
    {
        if (Data.DropDate == NewData.RelicData.DropDate)
        {
            Data = NewData.RelicData; // 데이터 업데이트
            bFound = true;
            break;
        }
    }
    if (!bFound)
    {
        RelicDataArray.Add(NewData.RelicData); // 없으면 추가
    }

    URelicSaveGame* SaveGameInstance;
    if (UGameplayStatics::DoesSaveGameExist(TEXT("RelicSaveSlot"), 0))
    {
        SaveGameInstance = Cast<URelicSaveGame>(UGameplayStatics::LoadGameFromSlot(TEXT("RelicSaveSlot"), 0));
    }
    else
    {
        SaveGameInstance = Cast<URelicSaveGame>(UGameplayStatics::CreateSaveGameObject(URelicSaveGame::StaticClass()));
    }
    if (SaveGameInstance)
    {
        // 2. SaveGameInstance->RelicSaveArray도 동일하게 처리
        bool bSaveFound = false;
        for (FRelicSaveData& SaveData : SaveGameInstance->RelicSaveArray)
        {
            if (SaveData.RelicData.DropDate == NewData.RelicData.DropDate)
            {
                SaveData = NewData; // 데이터 업데이트
                bSaveFound = true;
                break;
            }
        }
        if (!bSaveFound)
        {
            SaveGameInstance->RelicSaveArray.Add(NewData); // 없으면 추가
        }
        UGameplayStatics::SaveGameToSlot(SaveGameInstance, TEXT("RelicSaveSlot"), 0);
    }
}
