#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CMuseumComponent.generated.h"

class UInputMappingContext;
class UEnhancedInputComponent;
class UInputAction;
class AMH_VRPlayer;
struct FInputActionInstance;

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

	UCMuseumComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	void SwitchState();
	
	void OnMenuButtonClicked();
	void OnSelectItemButtonClicked(const FInputActionInstance& IA_Instance);
};
