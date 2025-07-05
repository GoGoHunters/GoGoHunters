#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CContinentWidgetActor.generated.h"

class UWidgetComponent;
class UCContinentWidget;
struct FCContinentData;

UCLASS()
class GOGOHUNTERS_API ACContinentWidgetActor : public AActor
{
	GENERATED_BODY()
	
public:
	void SetContinentData(const FCContinentData& ContinentData);

private:
	ACContinentWidgetActor();
	virtual void BeginPlay() override;	

	UPROPERTY(EditDefaultsOnly)
	USceneComponent* RootScene;
	UPROPERTY(EditDefaultsOnly)
	UWidgetComponent* WidgetComponent;
	UPROPERTY()
	TObjectPtr<UCContinentWidget> ContinentWidget;
};
