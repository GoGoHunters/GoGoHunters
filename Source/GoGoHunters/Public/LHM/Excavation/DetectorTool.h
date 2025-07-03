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

public:
	void StartDetection();
	void StopDetection();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UStaticMeshComponent* DetectorMesh;

	UPROPERTY()
	class ARelicsBase* TargetArtifact;
	UPROPERTY()
	class UDetectionComponent* DetectionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UWidgetComponent* DetectionWidgetComp;
	UPROPERTY()
	TSubclassOf<class UDetectionUI> DetectionUIClass;
	UPROPERTY()
	class UDetectionUI* DetectionUI;

	UPROPERTY()
	TSubclassOf<class AAI_Docent> DocentClass;
	UPROPERTY()
	class AAI_Docent* AI_Docent;

	float DetectionProgress = 0.0f;
	bool bIsDetecting = false;

	void UpdateDetection(float DeltaTime);
};
