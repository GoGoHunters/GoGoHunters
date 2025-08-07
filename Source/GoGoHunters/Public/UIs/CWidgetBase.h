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

	/////
	UFUNCTION()
	void PlaySystemMessageAnim();
	
	UPROPERTY(meta = (BindWidgetAnimOptional), Transient)
	UWidgetAnimation* Anim_ShowMessage;
	
	
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
	//MainUI//////
	UPROPERTY(meta=(BindWidgetOptional))
	class UCanvasPanel* Can_SystemMessage;
	
	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UButton> Btn_Tami;
	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UButton> Btn_Lobby;
	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UButton> Btn_Settings;
	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UButton> Btn_Exit;


	UFUNCTION()
	void OnBtnTamiClicked();
	UFUNCTION()
	void OnBtnLobbyClicked();
	UFUNCTION()
	void OnBtnSetClicked();
	UFUNCTION()
	void OnBtnExitClicked();

	UFUNCTION()
	void OnShowMessage();
	UFUNCTION()
	void OnHideMessage();
	///////
	
	UFUNCTION()
	void OnBtnAClicked();
	UFUNCTION()
	void OnBtnBClicked();

	void SwitchUI(const int32 InDesiredUiIdx);
};
