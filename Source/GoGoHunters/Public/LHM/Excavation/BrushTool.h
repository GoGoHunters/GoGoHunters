// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BrushTool.generated.h"

UCLASS()
class GOGOHUNTERS_API ABrushTool : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABrushTool();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* Overlapped, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 BodyIndex, bool bFromSweep, const FHitResult& Hit);
	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* Overlapped, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 BodyIndex);

private:
	void CheckBrushSwipe(float DeltaTime);

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UStaticMeshComponent* BrushMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UBoxComponent* BoxMesh;

	UPROPERTY()
	class AActor* CurrentOverlappingRelic;

	// 최소 스와이프 속도
	UPROPERTY(EditAnywhere)
	float BrushSwipeThreshold = 20.0f;

	// 데칼 페이드 속도
	UPROPERTY(EditAnywhere)
	float FadeSpeed = 0.3f;

	FVector PreviousLocation;
	float SwipeSpeed = 0.0f;

	void SetIsBrushing(bool _bIsBrushing) { bIsBrushing = _bIsBrushing; }
	bool bIsBrushing = false;
};
