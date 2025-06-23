#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CGlobe.generated.h"

UCLASS()
class GOGOHUNTERS_API ACGlobe : public AActor
{
	GENERATED_BODY()
	
public:	
	ACGlobe();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};
