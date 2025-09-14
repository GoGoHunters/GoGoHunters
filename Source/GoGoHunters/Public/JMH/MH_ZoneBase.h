// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
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
	// 진입 시 출력할 가이드 메시지
	UPROPERTY(EditAnywhere, Category = "Zone")
	FString GuideMessage;

	// 콜리전 판정용 스피어 컴포넌트
	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* CollisionBox;

	UPROPERTY(EditAnywhere, Category = "Zone")
	FText PromptText;

	UPROPERTY(EditAnywhere, Category = "Zone")
	FString TargetLevelName;

	UPROPERTY(EditAnywhere, Category = "Zone")
	TSubclassOf<class UMH_MessageUI> MessageUIClass;

	UPROPERTY()
	class UMH_MessageUI* MessageUI;

	UFUNCTION()
	void ShowZoneMessageUI(FString Message);

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

	UFUNCTION()
	void OnZoneOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	

public:
	//위젯 비지블 안보이게
	UFUNCTION()
	void HandleMessageUIClose();
	// 존 실행용 내부 함수들
	UFUNCTION()
	void HandleMapInteraction();
	UFUNCTION()
	void HandleRestoreInteraction();
	UFUNCTION()
	void HandleMyMuseumInteraction();
	UFUNCTION()
	void HandleRecordInteraction();
	UFUNCTION()
	void HandleSettingsInteraction();
	UFUNCTION()
	void HandleExitInteraction();
	UFUNCTION()
	void HandleLobbyInteraction();
	UFUNCTION()
	void HandleRestoreListInteraction();
	

	UPROPERTY()
	class AMH_VRPlayer* VRPlayer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UWidgetComponent* MessageWidgetComponent;

	UPROPERTY(EditDefaultsOnly)
	UWidgetComponent* RestoreListComponent;
	

	
};
