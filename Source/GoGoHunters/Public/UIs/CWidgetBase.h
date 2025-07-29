#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CWidgetBase.generated.h"

class ACUiActor;
class UButton;
/**
 * 
 */
UCLASS()
class GOGOHUNTERS_API UCWidgetBase : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetUiActor(ACUiActor* InActor) { UiActor = InActor; }

protected:
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UButton> Btn_A;
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UButton> Btn_B;
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<ACUiActor> UiActor;

private:
	virtual void NativeConstruct() override;
	
	UFUNCTION()
	void OnBtnAClicked();
	UFUNCTION()
	void OnBtnBClicked();

	void SwitchUI(const int32 InDesiredUiIdx);
};
