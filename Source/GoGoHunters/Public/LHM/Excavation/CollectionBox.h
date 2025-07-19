// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CollectionBox.generated.h"

UCLASS()
class GOGOHUNTERS_API ACollectionBox : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACollectionBox();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* Overlapped, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit);

public:
	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* CollectionBox;

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* TriggerVolume;

	UPROPERTY(EditAnywhere)
	class USoundBase* CollectionSFX;

	UFUNCTION()
	void SetTargetRelic(class ARelicsBase* NewTargetRelic) { TargetRelic = NewTargetRelic; }
	
	UFUNCTION()
	void SetRelicsManager(class ARelicsManager* NewRelicsManager) { RelicsManager = NewRelicsManager; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class ARelicsBase* TargetRelic = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class ARelicsManager* RelicsManager = nullptr;

	UPROPERTY()
	TSet<UStaticMeshComponent*> CollectedMeshes;

	UFUNCTION()
	void CheckAllCollected();

	UFUNCTION()
	void PlayBoxCloseAnimation();
};
