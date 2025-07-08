#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CWorldMap.generated.h"

class ACContinentWidgetActor;
class UDataTable;
struct FCContinentData;

UCLASS()
class GOGOHUNTERS_API ACWorldMap : public AActor
{
	GENERATED_BODY()
	
public:
	void EnableCompOutline(UStaticMeshComponent* Comp);
	void ResetPrevOutline();
	
private:
	UPROPERTY(EditDefaultsOnly)
	USceneComponent* RootScene;
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* Korea;
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* NorthAmerica;
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* SouthAmerica;
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* Asia;
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* Oceania;
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* Europe;
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* Africa;
	UPROPERTY(EditDefaultsOnly)
	UChildActorComponent* ContinentWidget;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UDataTable> ContinentDataTable;
	TArray<FCContinentData> ContinentDataArray;
	
	UPROPERTY()
	TObjectPtr<ACContinentWidgetActor> ContinentWidgetActor = nullptr;
	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> PrevOutlinedComp = nullptr;
	
	ACWorldMap();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void SetComponentInit(UStaticMeshComponent* Comp);
	const FCContinentData GetContinentData(const FString& ContinentName);
};
