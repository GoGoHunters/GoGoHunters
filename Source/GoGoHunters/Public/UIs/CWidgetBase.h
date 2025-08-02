#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CWidgetBase.generated.h"

class ACUiActor;
class UButton;
class UImage;
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
	UFUNCTION(BlueprintCallable)
	void BlindActive(bool bUseBlind);
	UFUNCTION(BlueprintNativeEvent)
	void K2_PlayUiAnim(bool bIsReverse);
	
protected:
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UButton> Btn_A;
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UButton> Btn_B;
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UImage> Img_Blind;
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<ACUiActor> UiActor;

	virtual void NativeConstruct() override;
private:
	
	UFUNCTION()
	void OnBtnAClicked();
	UFUNCTION()
	void OnBtnBClicked();

	void SwitchUI(const int32 InDesiredUiIdx);
};
