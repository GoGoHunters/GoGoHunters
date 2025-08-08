// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExcavationManager.generated.h"

// 발굴 단계 열거형
UENUM(BlueprintType)
enum class EExcavationPhase : uint8
{
	Detection     UMETA(DisplayName = "탐지 단계"),
	Digging       UMETA(DisplayName = "삽질 단계"),
	Brushing      UMETA(DisplayName = "붓질 단계"),
	Collection    UMETA(DisplayName = "수거 단계"),
	Completed     UMETA(DisplayName = "완료")
};

// 발굴 단계 변경 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnExcavationPhaseChanged, EExcavationPhase, NewPhase);

UCLASS()
class GOGOHUNTERS_API AExcavationManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AExcavationManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	// 탐지된 유물에 대한 발굴 시작 지시
	void NotifyDetectionCompleted(class ARelicsManager* Target);

	// 붓단계 진입 콜백 (RelicsManager → 통보)
	void NotifyExcavationCompleted(class ARelicsManager* Target);

	// 붓질 완료 (데칼 제거 완료)
	void NotifyDustingCompleted(class ARelicsManager* Target);

	// 유물 수거 완료
	void NotifyCollectionCompleted(class ARelicsManager* Target, class ACollectionBox* CollectionBox);

	// 현재 발굴 단계 반환
	UFUNCTION(BlueprintCallable, Category = "Excavation")
	EExcavationPhase GetCurrentPhase() const { return CurrentPhase; }

	// 발굴 단계 변경
	UFUNCTION(BlueprintCallable, Category = "Excavation")
	void SetCurrentPhase(EExcavationPhase NewPhase);

	// 특정 단계에서 사용 가능한 도구인지 확인
	UFUNCTION(BlueprintCallable, Category = "Excavation")
	bool IsToolAvailableForPhase(int32 ToolIndex) const;

	// 발굴 단계 변경 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "Excavation")
	FOnExcavationPhaseChanged OnExcavationPhaseChanged;

protected:
	UPROPERTY()
	TArray<class ARelicsManager*> AllRelicsManagers;

	UPROPERTY()
	class ARelicsManager* CurrentActiveManager;
	
	UPROPERTY()
	class ACollectionBox* CollectionBox;

	// 현재 발굴 단계
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Excavation")
	EExcavationPhase CurrentPhase = EExcavationPhase::Detection;

	// 단계별 사용 가능한 도구 설정
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Excavation")
	TArray<bool> ToolAvailabilityByPhase;

public:
	void ChangeExcavationPhase();
	void ChangeCompletedPhase();
	void PlayPopupUiAnim(bool IsTunrOff);

// 발굴 UI 업데이트를 위한 DiggingUI/BrushingUI 참조
public:
	void SetPhaseUI(class UExcavationPhaseUI* InPhaseUI) { PhaseUI = InPhaseUI; }

	void SetDiggingUI(class UDiggingUI* InDiggingUI) { DiggingUI = InDiggingUI; }
	void UpdateDiggingProgress();

	void SetBrushingUI(class UBrushingUI* InBrushingUI) { BrushingUI = InBrushingUI; }

protected:
    UPROPERTY()
    class UExcavationPhaseUI* PhaseUI;

    UPROPERTY()
    class UDiggingUI* DiggingUI;
	
    UPROPERTY()
    class UBrushingUI* BrushingUI;

// 타미 대사
private:
	void PlayTami(const FName& FunctionName);
};
