#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LHJ/CRelicData.h"
#include "CMuseumComponent.generated.h"

class UInputMappingContext;
class UEnhancedInputComponent;
class UInputAction;
class AMH_VRPlayer;
class ACRelicBase;
struct FInputActionInstance;
struct FCRelicData;
struct FCRelicDetailData;

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
	void SetupPlayerInputComponent(UEnhancedInputComponent* EnhancedInput);
	void PlayPreviewMode(const FCRelicData& InRelicData, const FCRelicDetailData& InRelicDetailData);
	void PlaceRelic();

	UFUNCTION(BlueprintCallable)
	void RegisterRelic();

private:
	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputMappingContext> IMC_Museum;
	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> IA_Menu;
	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> IA_SelectItem_R;
	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> IA_SelectItem_L;

	UPROPERTY()
	TObjectPtr<AMH_VRPlayer> OwnerPlayer;
	EMuseumState MuseumState = EMuseumState::Display;

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

	UCMuseumComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	void SwitchState();

	UFUNCTION()
	void OnMenuButtonClicked();

	void PreviewMode();
	void PreviewEnd();
};
