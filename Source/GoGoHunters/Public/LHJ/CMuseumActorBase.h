#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CMuseumActorBase.generated.h"

UCLASS()
class GOGOHUNTERS_API ACMuseumActorBase : public AActor
{
	GENERATED_BODY()
	
public:	

protected:
	ACMuseumActorBase();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
};
