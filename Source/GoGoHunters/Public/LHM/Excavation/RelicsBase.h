// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RelicsBase.generated.h"

UCLASS()
class GOGOHUNTERS_API ARelicsBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARelicsBase();

protected:
	virtual void PostInitializeComponents() override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<class UStaticMeshComponent*> RelicsMeshes;

// RelicsManager 참조
	UFUNCTION()
	void SetRelicsManager(class ARelicsManager* NewRelicsManager) { RelicsManager = NewRelicsManager; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Digging Events")
	class ARelicsManager* RelicsManager;

	// 유물 태그 설정
	UFUNCTION(BlueprintCallable, Category = "Relics")
	void SetRelicTag(int32 InRelicTag) { RelicTag = InRelicTag; }

	// 유물 태그 반환
	UFUNCTION(BlueprintCallable, Category = "Relics")
	int32 GetRelicTag() const { return RelicTag; }

// 탐지
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AExcavationMarker> MarkerClass;

	UPROPERTY()
	class AExcavationMarker* Marker;

// 붓 데칼
	UPROPERTY(VisibleAnywhere)
	TArray<class UDecalComponent*> DustDecals;

	// 데칼 → 메시 매핑
	UPROPERTY()
	TMap<class UDecalComponent*, class UStaticMeshComponent*> DecalToMeshMap;

	// 데칼 → 머티리얼 인스턴스
	UPROPERTY()
	TMap<class UDecalComponent*, class UMaterialInstanceDynamic*> DecalMIDs;

	UPROPERTY(EditAnywhere)
	FName OpacityParameterName = TEXT("Opacity");

	UPROPERTY(EditAnywhere)
	float CurrentOpacity = 1.0f;

	UPROPERTY(EditAnywhere)
	class USoundBase* DecalRemovalSFX;

// 유물 태그 정보
	UPROPERTY(EditAnywhere, Category="Relics")
	int32 RelicTag = -1;

	void ReduceDustOpacity(const FVector& BrushLocation, float Amount, class ABrushTool& BrushRef);

	void CheckAllDelcalsRemoved();

// BrushingUI
public:
	void SetBrushingUI(class UBrushingUI* InBrushingUI);

	UStaticMeshComponent* GetRelicMeshByDecal(UDecalComponent* Decal) const;
	bool HasValidMID(UDecalComponent* Decal) const;
	float GetDecalOpacity(UDecalComponent* Decal) const;

protected:
	UPROPERTY()
	class UBrushingUI* BrushingUI;
};
