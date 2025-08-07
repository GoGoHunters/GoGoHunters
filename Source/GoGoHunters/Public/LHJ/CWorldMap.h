#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CContinentData.h"
#include "CWorldMap.generated.h"

class ACContinentWidgetActor;

UCLASS()
class GOGOHUNTERS_API ACWorldMap : public AActor
{
	GENERATED_BODY()
	
public:
	void EnableCompOutline(UStaticMeshComponent* Comp);
	void ResetPrevOutline();
	void SetContinentVisibleHidden();
	
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

	UPROPERTY(EditAnywhere, Category = Color, meta = (AllowPrivateAccess))
	FLinearColor RecoverAdditiveChangeColor = FLinearColor(0, 0, 0);
	UPROPERTY(EditAnywhere, Category = Color, meta = (AllowPrivateAccess))
	FLinearColor ActiveAdditiveChangeColor = FLinearColor(.7, .7, .7);
	
	ACWorldMap();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void SetComponentInit(UStaticMeshComponent* Comp);
	const FCContinentData GetContinentData(const FString& ContinentName);
};
