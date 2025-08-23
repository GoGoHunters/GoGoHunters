#pragma once

#include "CoreMinimal.h"
#include "CRelicData.h"
#include "GameFramework/Actor.h"
#include "CRelicDescActor.generated.h"

struct FCRelicDetailData;
class UWidgetComponent;

UCLASS()
class GOGOHUNTERS_API ACRelicDescActor : public AActor
{
	GENERATED_BODY()
	
public:
	void UpdateDescriptionWidget(bool bUpdate, FCRelicData InRelicData = FCRelicData(), FCRelicDetailData InRelicDetailData = FCRelicDetailData());
	
private:
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* MeshComp;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* DescWidget;
	
	ACRelicDescActor();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

};
