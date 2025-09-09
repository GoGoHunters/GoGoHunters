#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CAlertMsgWidget.generated.h"

class AAC_KeyBoard;
class UButton;
class UTextBlock;
class UWidgetSwitcher;
/**
 * 
 */
UCLASS()
class GOGOHUNTERS_API UCAlertMsgWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetInitialText(const FString InText);
	void SetKeyboard(AAC_KeyBoard* InKeyboard) {KeyBoard = InKeyboard;}
	
private:
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> Switcher;
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> TxtInputText;
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UButton> BtnOk;
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UButton> BtnNo;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> TxtInputText2;
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UButton> BtnClose;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> TxtInputText3;
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UButton> BtnClose2;

	UPROPERTY()
	TObjectPtr<AAC_KeyBoard> KeyBoard;

	FString InitialText;
	bool bRequest = false;

	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnClickBtnOk();
	UFUNCTION()
	void OnClickBtnClose();
	UFUNCTION()
	void OnClickClose();
};
