#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CWorldMap.generated.h"

UCLASS()
class GOGOHUNTERS_API ACWorldMap : public AActor
{
	GENERATED_BODY()
	
public:
	void EnableCompOutline(UStaticMeshComponent* Comp, bool bEnable);
	
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
	
	ACWorldMap();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void SetComonentInit(UStaticMeshComponent* Comp);
};
