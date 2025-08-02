#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CUiActor.generated.h"

class UCRelicCollectionWidget;
class AMH_VRPlayer;

UCLASS()
class GOGOHUNTERS_API ACUiActor : public AActor
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SwitchActiveWidget(const int32 InDesiredUiIdx);

	UFUNCTION(BlueprintImplementableEvent, Category = "UI Event")
	void K2_PlayPopupUiAnim(bool IsTurnOff);

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	TObjectPtr<AMH_VRPlayer> OwnerPlayer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	TObjectPtr<UCRelicCollectionWidget> RelicCollectionWidget;

	ACUiActor();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void SwitchActiveWidget_Implementation(const int32 InDesiredUiIdx)
	{
	}

	UFUNCTION(BlueprintCallable)
	void BindMuseumDelegate();
};
