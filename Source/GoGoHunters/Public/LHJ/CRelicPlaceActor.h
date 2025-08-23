#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CRelicPlaceActor.generated.h"

class ACRelicBase;
class UCMuseumComponent;
class UBoxComponent;

UCLASS()
class GOGOHUNTERS_API ACRelicPlaceActor : public AActor
{
	GENERATED_BODY()

public:
	bool CanPlaceRelic() { return bRegisterRelic; }
	void RegisterRelic(const ACRelicBase* InRegisterRelic);
	void UnRegisterRelic(const ACRelicBase* InUnRegisterRelic);

	FVector GetPlaceMeshScale() const { return PlaceMeshScale; }
	
private:
	UPROPERTY()
	USceneComponent* RootScene;
	UPROPERTY(EditDefaultsOnly, Category="Param|Component", BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	UBoxComponent* DetectCollision;
	UPROPERTY(EditDefaultsOnly, Category="Param|Component", BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	UStaticMeshComponent* PlaceMesh;

	UPROPERTY(VisibleAnywhere, Category = "Param|DetectCollision", meta = (ToolTip = "이 카테고리는 DetectCollision의 Transform 설정을 위한 파라미터들을 모아둡니다."))
	bool bDetectCollisionCategoryDescriptionHelper;
	UPROPERTY(EditAnywhere, Category="Param|DetectCollision", BlueprintReadOnly, meta=(AllowPrivateAccess=true, ToolTip="DetectCollision 위치 설정"))
	FVector DetectCollisionLocation = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, Category="Param|DetectCollision", BlueprintReadOnly, meta=(AllowPrivateAccess=true, ToolTip="DetectCollision 회전 설정"))
	FRotator DetectCollisionRotation = FRotator::ZeroRotator;
	UPROPERTY(EditAnywhere, Category="Param|DetectCollision", BlueprintReadOnly, meta=(AllowPrivateAccess=true, ToolTip="DetectCollision 크기 설정"))
	FVector DetectCollisionSize = FVector(1.2, 1.2, 1);
	
	UPROPERTY(VisibleAnywhere, Category = "Param|PlaceMesh", meta = (ToolTip = "이 카테고리는 PlaceMesh의 Transform 설정을 위한 파라미터들을 모아둡니다."))
	bool bPlaceMeshCategoryDescriptionHelper;
	UPROPERTY(EditAnywhere, Category="Param|PlaceMesh", BlueprintReadOnly, meta=(AllowPrivateAccess=true, ToolTip="PlaceMesh 위치 설정"))
	FVector PlaceMeshLocation = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, Category="Param|PlaceMesh", BlueprintReadOnly, meta=(AllowPrivateAccess=true, ToolTip="PlaceMesh 회전 설정"))
	FRotator PlaceMeshRotation = FRotator::ZeroRotator;
	UPROPERTY(EditAnywhere, Category="Param|PlaceMesh", BlueprintReadOnly, meta=(AllowPrivateAccess=true, ToolTip="PlaceMesh 크기 설정"))
	FVector PlaceMeshSize = FVector(.7, .7, .1);

	UPROPERTY(EditAnywhere, Category="Param|Relic" ,meta=(ToolTip="배치한 유물의 크기 설정"))
	FVector PlaceMeshScale = FVector(1, 1, 1);

	UPROPERTY(VisibleAnywhere, Category="Param|Settings" ,meta=(ToolTip="유물 배치 여부"))
	bool bRegisterRelic = false;
	
	UPROPERTY()
	TObjectPtr<UCMuseumComponent> MuseumComp;		
	
	ACRelicPlaceActor();
	virtual void RerunConstructionScripts() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
};
