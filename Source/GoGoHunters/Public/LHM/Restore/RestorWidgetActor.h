// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RestorWidgetActor.generated.h"

UCLASS()
class GOGOHUNTERS_API ARestorWidgetActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARestorWidgetActor();

	//class UWidgetComponent* GetCompleteWidgetComp() const { return CompleteWidgetComp; }
	void ShowCompleteUI(bool bShow);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:
    UPROPERTY(VisibleAnywhere)
    class USceneComponent* RootScene;

    UPROPERTY(VisibleAnywhere)
    class UStaticMeshComponent* BookMesh;

    UPROPERTY(VisibleAnywhere)
    class UWidgetComponent* RestoreWidgetComp;

    UPROPERTY(VisibleAnywhere)
    class UWidgetComponent* CompleteWidgetComp;

};
