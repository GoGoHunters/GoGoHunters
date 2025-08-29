#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LHJ/CRelicData.h"
#include "CMuseumComponent.generated.h"

class ACRelicPlaceActor;
class ACMuseumPlaceArea;
class UInputMappingContext;
class UEnhancedInputComponent;
class UInputAction;
class AMH_VRPlayer;
class ACRelicBase;
class UMH_GrabComp;
struct FInputActionInstance;
struct FCRelicData;
struct FCRelicDetailData;

DECLARE_DELEGATE(FRelicPlaceDel);
DECLARE_DELEGATE_OneParam(FUiAnimPlay, bool);
DECLARE_DYNAMIC_DELEGATE(FMakeGridCompleted);

UENUM(BlueprintType)
enum EMuseumState : uint8
{
	Display = 0 UMETA(DisplayName = "전시 모드"),
	Decorate UMETA(DisplayName = "꾸미기 모드"),
	Max UMETA(Hidden)
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GOGOHUNTERS_API UCMuseumComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	FRelicPlaceDel OnRelicPlace;
	FUiAnimPlay OnUiAnimPlay;
	FMakeGridCompleted OnMakeGridCompleted;

	bool bBeginPlayEnded = false;

	UFUNCTION(BlueprintCallable)
	const EMuseumState GetMuseumState() { return MuseumState; }

	bool IsPreviewMode() { return bIsPreviewMode; }

	void SetupPlayerInputComponent(UEnhancedInputComponent* EnhancedInput);
	void PlayPreviewMode(const FCRelicData& InRelicData, const FCRelicDetailData& InRelicDetailData);
	void PlaceRelic();

	UFUNCTION(BlueprintCallable)
	FCRelicData RegisterRelic(const int32& InRelicTag = -1);
	void RecoverRelic(FCRelicData& InRelicData);
	UFUNCTION(BlueprintCallable)
	void RegisterRelicCollector(FCRelicData& InRelicData, FName InCollectorName = NAME_None);
	void GrabRelic(ACRelicBase* GrabRelic);
	void GrabRelicEnd(ACRelicBase* GrabRelic, const FVector& HandComponentLocation);

	UFUNCTION(BlueprintCallable)
	void SwitchState();

private:
	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputMappingContext> IMC_Museum;
	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> IA_Menu;

	UPROPERTY()
	TObjectPtr<AMH_VRPlayer> OwnerPlayer;
	EMuseumState MuseumState = EMuseumState::Display;
	UPROPERTY(EditDefaultsOnly)
	FString MuseumLevelName = TEXT("Museum");

	UPROPERTY()
	TObjectPtr<ACRelicPlaceActor> PlaceArea = nullptr;
	UPROPERTY()
	TObjectPtr<AActor> SelectedActor = nullptr; // 선택된 오브젝트 저장

	FCRelicData RelicData;
	FCRelicDetailData RelicDetailData;
	UPROPERTY()
	TObjectPtr<ACRelicBase> Relic = nullptr;
	bool bIsPreviewMode = false;
	bool bCanPlace = false;

	// 생성된 다이나믹 머터리얼 인스턴스 관리
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> RelicDynamicMaterial = nullptr;

	FTransform BuildTransform;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UMaterialInterface> RelicAcceptMaterial;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UMaterialInterface> RelicRejectedMaterial;

	UPROPERTY(EditAnywhere)
	float RePlaceAreaSearchRange = 200.f;
	UPROPERTY()
	TObjectPtr<UMH_GrabComp> GrabComponent;

	// 손에 잡았을 때, 유물 크기
	UPROPERTY(EditDefaultsOnly)
	FVector GrabRelicScale = FVector(.4);
	UPROPERTY(EditDefaultsOnly)
	float LerpScale = 0.1f;
	bool bIsGrabbing = false;
	UPROPERTY()
	TObjectPtr<ACRelicBase> GrabbedRelic = nullptr;

	int32 MakeGridCompletedCount = 0;

	UCMuseumComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void ToggleMenu();
	UFUNCTION()
	void OnMenuButtonClicked();

	void PreviewMode();
	void PreviewEnd();

	// Data.PlaceArea를 신뢰하지 않고, 저장된 PlacedTransform 위치 기반으로 반경 내 PlaceArea와 셀 스케일을 찾는 헬퍼
	bool FindNearbyPlaceArea(const FVector& Location, float SearchRadius, ACRelicPlaceActor*& OutArea,
	                         FVector& OutCellScale) const;

	void SetRelicScaleToGrabScale();

	UFUNCTION()
	void LoadPlacedRelic();
};
