#pragma once

#include "CoreMinimal.h"
#include "CMuseumActorBase.h"
#include "CStandBase.generated.h"

UCLASS()
class GOGOHUNTERS_API ACStandBase : public ACMuseumActorBase
{
	GENERATED_BODY()
	
public:	

	
protected:
	ACStandBase();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

};
