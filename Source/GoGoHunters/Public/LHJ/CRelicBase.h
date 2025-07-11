#pragma once

#include "CoreMinimal.h"
#include "CMuseumActorBase.h"
#include "CRelicData.h"
#include "CRelicBase.generated.h"

UCLASS()
class GOGOHUNTERS_API ACRelicBase : public ACMuseumActorBase
{
	GENERATED_BODY()
	
public:	
	UStaticMeshComponent* GetRelicMesh() const { return RelicMesh; }
	void SetRelicMaterial(UMaterialInterface* NewRelicMaterial);
	void InitializeAsset(const FCRelicData& InRelicData, const FCRelicDetailData& InRelicDetailData);
	
protected:
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* RelicMesh;
	UPROPERTY()
	TObjectPtr<UMaterialInterface> RelicMaterial;

	FCRelicData RelicData;
	FCRelicDetailData RelicDetailData;
		
	ACRelicBase();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

};
