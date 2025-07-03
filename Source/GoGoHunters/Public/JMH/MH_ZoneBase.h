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
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnPlayerInteracted(AActor* Player);
	virtual  void OnPlayerInteracted_Implementation(AActor* Player);

protected:
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Zone")
	FName ZoneTag;
	
	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* ZoneVisual;
	
	// 진입 시 출력할 가이드 메시지
	UPROPERTY(EditAnywhere, Category = "Zone")
	FString GuideMessage;

	// 콜리전 판정용 스피어 컴포넌트
	UPROPERTY(VisibleAnywhere)
	class USphereComponent* CollisionSphere;

private:
	UFUNCTION()
	void OnZoneOverlapBegin(
		UPrimitiveComponent* OverlappedComp, 
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex,
		bool bFromSweep, 
		const FHitResult& SweepResult
	);

public:
	// 존 실행용 내부 함수들
	UFUNCTION()
	void HandleGlobeInteraction(AActor* Player);
	UFUNCTION()
	void HandleRestoreInteraction(AActor* Player);
	UFUNCTION()
	void HandleMyMuseumInteraction(AActor* Player);
	UFUNCTION()
	void HandleRecordInteraction(AActor* Player);
	UFUNCTION()
	void HandleSettingsInteraction(AActor* Player);
	UFUNCTION()
	void HandleExitInteraction(AActor* Player);

	
};
