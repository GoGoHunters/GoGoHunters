#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CMuseumPlaceArea.generated.h"

class UBoxComponent;

UCLASS()
class GOGOHUNTERS_API ACMuseumPlaceArea : public AActor
{
	GENERATED_BODY()
	
public:	

private:
	UPROPERTY()
	UBoxComponent* BoxComponent;
	
	ACMuseumPlaceArea();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
};
