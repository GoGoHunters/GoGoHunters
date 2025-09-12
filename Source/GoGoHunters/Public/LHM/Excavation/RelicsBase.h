// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RelicsBase.generated.h"

// 데칼 스냅샷
//USTRUCT()
//struct FDecalSnapshot
//{
//	GENERATED_BODY()
//
//	UPROPERTY() TSubclassOf<UDecalComponent> DecalClass;
//	UPROPERTY() TWeakObjectPtr<USceneComponent> AttachParent;
//	UPROPERTY() FTransform RelativeTransform;
//	UPROPERTY() UMaterialInterface* SourceMat = nullptr;
//	UPROPERTY() float InitialOpacity = 1.f;
//};

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

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* Overlapped, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit);

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<class UStaticMeshComponent*> RelicsMeshes;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UBoxComponent* TriggerVolume;

// RelicsManager 참조
public:
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
	
	class AExcavationMarker* GetMarker() const { return Marker; }

	void ActivateMarker();

// 유물 태그 정보
protected:
	UPROPERTY(EditAnywhere, Category="Relics")
	int32 RelicTag = -1;
	
// 탐지
public:
	bool GetIsDetected() const { return bIsDetected; }
	void SetIsDetected(bool bDetected) { bIsDetected = bDetected; }
protected:
	bool bIsDetected = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AExcavationMarker> MarkerClass;

	UPROPERTY()
	class AExcavationMarker* Marker;

// 붓 데칼
public:
	UStaticMeshComponent* GetClosestRelicMesh(const FVector& BrushLocation) const;
	void ReduceDustOpacity(const FVector& BrushLocation, float Amount, class ABrushTool& BrushRef);
	void CheckAllDelcalsRemoved();

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

// BrushingUI
public:
	void SetBrushingUI(class UBrushingUI* InBrushingUI) { BrushingUI = InBrushingUI; }
	UBrushingUI* GetBrushingUI() const { return BrushingUI; }

	UStaticMeshComponent* GetRelicMeshByDecal(UDecalComponent* Decal) const;
	bool HasValidMID(UDecalComponent* Decal) const;
	float GetDecalOpacity(UDecalComponent* Decal) const;

	float TotalInitialOpacity = 0.f;
	float TotalRemainingOpacity = 0.f;

protected:
	UPROPERTY()
	class UBrushingUI* BrushingUI;

// Warning
public:
	UFUNCTION() 
	void ResetDecalsAndProgress();   // 경고 3회 → 3초 뒤 호출
//protected:
//	UPROPERTY() 
//	TArray<FDecalSnapshot> InitialDecals; // 초기 스냅샷

public:
	void CountWarning();

	void SetWarningUI(class UWarningUI* InWarningUI) { WarningUI = InWarningUI; }
	UWarningUI* GetWarningUI() const { return WarningUI; }

protected:
	UPROPERTY()
	class UWarningUI* WarningUI;
	
	int32 WarningCount = 0;

// Tami 음성
private:
	void PlayTami();
	bool bIsPlayingTami = false;
	int32 NumDustingCompletedRelics = 0;

// 개발자 키
public:
	UFUNCTION(BlueprintCallable)
	void PressedDevKey();
};
