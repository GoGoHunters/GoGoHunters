#pragma once

#include "CoreMinimal.h"
#include "CMuseumActorBase.h"
#include "CRelicBase.generated.h"

UCLASS()
class GOGOHUNTERS_API ACRelicBase : public ACMuseumActorBase
{
	GENERATED_BODY()
	
public:	
	
	
protected:
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* RelicMesh;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UMaterial> RelicMaterial;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UMaterialInterface> RelicAcceptMaterial;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UMaterialInterface> RelicRejectedMaterial;	
	
	ACRelicBase();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

};
