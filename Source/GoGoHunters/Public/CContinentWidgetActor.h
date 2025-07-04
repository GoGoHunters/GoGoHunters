#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CContinentWidgetActor.generated.h"

UCLASS()
class GOGOHUNTERS_API ACContinentWidgetActor : public AActor
{
	GENERATED_BODY()
	
public:	

private:
	ACContinentWidgetActor();
	virtual void BeginPlay() override;	
	virtual void Tick(float DeltaTime) override;
};
