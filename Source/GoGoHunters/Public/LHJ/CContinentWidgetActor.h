#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CContinentWidgetActor.generated.h"

class UTextRenderComponent;
class UWidgetComponent;
class UCContinentWidget;
class ACWorldMap;
struct FCContinentData;

UCLASS()
class GOGOHUNTERS_API ACContinentWidgetActor : public AActor
{
	GENERATED_BODY()
	
public:
	void SetContinentData(const FCContinentData& ContinentData);
	void SetContinentVisibleHidden();
	void SetOuterActor(ACWorldMap* InOwner);

private:
	ACContinentWidgetActor();
	virtual void BeginPlay() override;	

	UPROPERTY(EditDefaultsOnly)
	USceneComponent* RootScene;
	UPROPERTY(EditDefaultsOnly)
	UWidgetComponent* WidgetComponent;
	UPROPERTY()
	TObjectPtr<UCContinentWidget> ContinentWidget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UTextRenderComponent* TitleTextComp;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UTextRenderComponent* DescTextComp;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UTextRenderComponent* RelicsTextComp;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UTextRenderComponent* MoveTextComp;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UTextRenderComponent* CloseTextComp;
	
	UPROPERTY()
	TObjectPtr<ACWorldMap> OuterOwner;

	FVector CloseTextLoc1=FVector(-16.7,-46,0);
	FVector CloseTextLoc2=FVector(0,-46,0);
};
