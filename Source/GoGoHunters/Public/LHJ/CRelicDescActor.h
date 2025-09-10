#pragma once

#include "CoreMinimal.h"
#include "CRelicData.h"
#include "GameFramework/Actor.h"
#include "CRelicDescActor.generated.h"

class UTextRenderComponent;
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
	UTextRenderComponent* TitleTextComp;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UTextRenderComponent* DescTextComp;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UTextRenderComponent* CollectorTextComp;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UTextRenderComponent* DateTextComp;
	
	ACRelicDescActor();
	void UpdateData(FCRelicDataParam Param);
	void VisibleComponent(bool bShown);
	FString FormatTextWithLineBreaks(const FString& Text, int32 MaxLength);
};
