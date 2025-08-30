#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CCollectingBook.generated.h"

class UWidgetComponent;

UCLASS()
class GOGOHUNTERS_API ACCollectingBook : public AActor
{
	GENERATED_BODY()
	
public:	
	UFUNCTION(BlueprintImplementableEvent)
	void ActiveAnim(bool bActive);
	
private:
	UPROPERTY(EditDefaultsOnly)
	USceneComponent* RootScene;
	UPROPERTY(EditDefaultsOnly)
	UWidgetComponent* CollectingBookWidget;
	
	ACCollectingBook();
	virtual void BeginPlay() override;	
	virtual void Tick(float DeltaTime) override;
};
