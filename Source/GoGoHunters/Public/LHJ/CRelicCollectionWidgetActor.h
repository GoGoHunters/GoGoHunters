#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CRelicCollectionWidgetActor.generated.h"

class UWidgetComponent;

UCLASS()
class GOGOHUNTERS_API ACRelicCollectionWidgetActor : public AActor
{
	GENERATED_BODY()
	
public:
	
private:
	UPROPERTY(EditDefaultsOnly)
	USceneComponent* RootScene;
	UPROPERTY(EditDefaultsOnly)
	UWidgetComponent* WidgetComponent;
	
	ACRelicCollectionWidgetActor();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
};
