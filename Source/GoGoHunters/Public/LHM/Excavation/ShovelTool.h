// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShovelTool.generated.h"

UCLASS()
class GOGOHUNTERS_API AShovelTool : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShovelTool();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ShovelTool")
	class UStaticMeshComponent* ShovelMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ShovelTool")
	class UBoxComponent* BoxComp;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ShovelTool")
	//float DiggingSpeed; // πﬂ±º º”µµ
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ShovelTool")
	//float DiggingDepth; // πﬂ±º ±Ì¿Ã
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ShovelTool")
	//bool bIsDigging; // πﬂ±º ¡ﬂ¿Œ¡ˆ ø©∫Œ
	

};
