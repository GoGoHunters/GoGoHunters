// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CollectionBox.generated.h"


USTRUCT()
struct FMovingPiece
{
	GENERATED_BODY()

	UPROPERTY() UStaticMeshComponent* Mesh = nullptr;
	UPROPERTY() FTransform Start;
	UPROPERTY() FTransform Target;
	UPROPERTY() float Elapsed = 0.f;
	UPROPERTY() float Duration = 0.35f;

	FMovingPiece() {}
	FMovingPiece(UStaticMeshComponent* InMesh, const FTransform& InStart, const FTransform& InTarget, float InDuration)
		: Mesh(InMesh), Start(InStart), Target(InTarget), Elapsed(0.f), Duration(InDuration) {}
};

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
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* TriggerVolume;

	UPROPERTY(EditAnywhere)
	class USoundBase* CollectionSFX;

	UFUNCTION()
	void SetTargetRelic(class ARelicsBase* NewTargetRelic) { TargetRelic = NewTargetRelic; }
	
	UFUNCTION()
	void SetRelicsManager(class ARelicsManager* NewRelicsManager) { RelicsManager = NewRelicsManager; }

	UFUNCTION()
	void SetInitialRelicTransforms(class ARelicsBase* Relic);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class ARelicsBase* TargetRelic = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class ARelicsManager* RelicsManager = nullptr;

	UPROPERTY()
	TSet<UStaticMeshComponent*> CollectedMeshes;

	UPROPERTY()
	TMap<UStaticMeshComponent*, FTransform> InitialRelicTransforms;

	UFUNCTION()
	void CheckAllCollected();

	UFUNCTION()
	void ResetCollectedRelics();

	UFUNCTION()
	void PlayBoxCloseAnimation();

	UFUNCTION(BlueprintImplementableEvent)
    void K2_CloseLid();

// 유물 스냅
public:
	/** Snap 포인트 루트 */
	UPROPERTY(VisibleAnywhere, Category="Snap")
	USceneComponent* SnapRoot;

	/** Snap 포인트 8개 (01~08) */
	UPROPERTY(VisibleAnywhere, Category="Snap")
	TArray<USceneComponent*> SnapPoints;

	/** 스냅 보간 시간(초) */
	UPROPERTY(EditAnywhere, Category="Snap")
	float SnapDuration = 0.35f;

protected:
	// Lerp 이동 중인 조각들
	UPROPERTY()
	TArray<FMovingPiece> MovingPieces;

	// 태그에서 Relic 인덱스(0~7) 파싱
	int32 GetRelicIndexFromTags(const UPrimitiveComponent* Comp) const;

	// 스냅 시작
	void StartSnapMove(UStaticMeshComponent* Mesh, int32 SnapIndex);

	// 스냅 진행(Tick)
	void TickSnapMoves(float DeltaTime);


// CollectionBox UI
public:
	void SetColleionCloseBtnUI(bool bVisible);

private:
	UPROPERTY(EditDefaultsOnly)
    class UWidgetComponent* WidgetComponent;

	bool bVisibleCloseBtnUI = false;

public:
// 개발자 키
	UFUNCTION(BlueprintCallable)
	void PressedDevKey();
};
