// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MH_ZoneBase.generated.h"

UCLASS()
class GOGOHUNTERS_API AMH_ZoneBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMH_ZoneBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual  void OnPlayerInteracted_Implementation(AMH_ZoneBase* Player);

protected:
	UPROPERTY(EditAnywhere, Category = "Zone")
	FName ZoneName;
	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* ZoneVisual;
	// 진입 시 출력할 가이드 메시지
	UPROPERTY(EditAnywhere, Category = "Zone")
	FString GuideMessage;

};
