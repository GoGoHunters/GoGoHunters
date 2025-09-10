// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RelicsManager.generated.h"

UCLASS()
class GOGOHUNTERS_API ARelicsManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARelicsManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
public:
	void StartExcavation(); // 초기 시작
	void NotifyGroundProgress(float Progress); // HeightMap 파괴량 보고

	UFUNCTION(BlueprintCallable)
	class ARelicsBase* GetRelics() const { return Relics; }

	UFUNCTION()
	ACollectionBox* GetCollectionBox() { return CollectionBox; }

protected:
	UPROPERTY(VisibleAnywhere)
	TArray<class UChildActorComponent*> GroundChildActors;
	
	UPROPERTY(VisibleAnywhere)
	class UChildActorComponent* GroundChild1;
	
	UPROPERTY(VisibleAnywhere)
	class UChildActorComponent* GroundChild2;
	
	UPROPERTY(VisibleAnywhere)
	class UChildActorComponent* GroundChild3;

	class ARelicsGround* Ground1;
	class ARelicsGround* Ground2;
	class ARelicsGround* Ground3;

	UPROPERTY(VisibleAnywhere)
	class UChildActorComponent* RelicsChild;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* ExcavationLand_02;

private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ARelicsGround> RelicsGroundClass;

	TArray<class ARelicsGround*> GroundLayers;

	UPROPERTY(EditAnywhere, Category="Relics")
	TSubclassOf<ARelicsBase> RelicsClass;

	class ARelicsBase* Relics;

	int32 CurrentLayerIndex = 0;

// 발굴 단계 트리거 버튼 위젯 / 발굴 진행률 위젯 액터
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AExcavationWidgetActor> PhaseUIActorClass;

	UPROPERTY()
	class AExcavationWidgetActor* PhaseUIActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AExcavationProgressWidgetActor> ProgressClass;

	UPROPERTY()
	class AExcavationProgressWidgetActor* ProgressActor;

private:
	bool bBrushPhaseStarted = false;

	UFUNCTION()
	void EnterBrushPhase();

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ACollectionBox> CollectionBoxClass;

	UPROPERTY()
	class ACollectionBox* CollectionBox;

public:
	UFUNCTION()
	void SpawnCollectionBox();

    // 현재 진행 중인 땅의 파괴도(0~50)를 반환
    bool GetCurrentDigProgress(float& OutProgress) const;

public:
	UFUNCTION()
	void SpawnKeyboard();

	UFUNCTION()
	void DestroyKeyboard();

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AAC_KeyBoard> KeyBoardClass;

	UPROPERTY()
	class AAC_KeyBoard* KeyBoardActor;

// 개발자 키
public:
	UPROPERTY(BlueprintReadWrite)
	bool bPressedDevKey = false;

// 타미 대사
private:
	void PlayTamiCompliments(int32 CurrentLayer, float Progress);
	void PlayTami(const FName& FunctionName);

	// 중복 재생 방지 플래그
	bool bPlayedCompliment2 = false;
	bool bPlayedCompliment3 = false;
	bool bPlayedVisibleRelic = false;
};
