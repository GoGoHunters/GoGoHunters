// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DetectorTool.generated.h"

UCLASS()
class GOGOHUNTERS_API ADetectorTool : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADetectorTool();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetIsDetecting(bool _bIsDetecting);

protected:
	void UpdateDetection(float DeltaTime);
	void StopDetection();

	void PlayTami1();
	void PlayTami2();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UStaticMeshComponent* DetectorMesh;

	UPROPERTY()
	class ARelicsBase* Relics;

protected:
	UPROPERTY()
	class UDetectionComponent* DetectionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UWidgetComponent* DetectionWidgetComp;

	float DetectionProgress = 0.0f;

// 나이아가라
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Effects")
	class UNiagaraComponent* VFX;

private:
	UPROPERTY()
	class UDetectionUI* DetectionUI;

	bool bIsDetecting = false;

// 개발자 키
public:
	UPROPERTY(BlueprintReadWrite)
	bool bPressedDevKey = false;
};
