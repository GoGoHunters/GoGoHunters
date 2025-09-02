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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
    UFUNCTION(BlueprintCallable)
    class URestoreUI* GetRestoreUI() const { return RestoreUI; }

protected:
    UPROPERTY(VisibleAnywhere, Category = "Components")
    class USceneComponent* RootScene;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    class UStaticMeshComponent* BookMesh;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    class UWidgetComponent* WidgetComponent;

    UPROPERTY()
    class URestoreUI* RestoreUI;

};
