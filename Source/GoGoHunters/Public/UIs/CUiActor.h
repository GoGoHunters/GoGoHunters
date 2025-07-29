#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CUiActor.generated.h"

UCLASS()
class GOGOHUNTERS_API ACUiActor : public AActor
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SwitchActiveWidget(const int32 InDesiredUiIdx);
	
private:
	ACUiActor();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void SwitchActiveWidget_Implementation(const int32 InDesiredUiIdx)	{	}
};
