#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CCollectingBook.generated.h"

class UCCollectingBookWidget;
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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* CollectingBookWidget;
	
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCCollectingBookWidget> CollectingBook;
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bStartPlayerDistCheck = false;
	UPROPERTY(EditDefaultsOnly, Category="Param|Dist")
	float Dist = 410.f;
	
	ACCollectingBook();
	virtual void BeginPlay() override;	
	virtual void Tick(float DeltaTime) override;

};
