#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LHJ/CContinentData.h"
#include "CContinentWidget.generated.h"

class USizeBox;
class UTextBlock;
class UButton;
class ACContinentWidgetActor;

/**
 * 
 */
UCLASS()
class GOGOHUNTERS_API UCContinentWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetContinentData(const FCContinentData& ContinentData);
	void SetOuterActor(ACContinentWidgetActor* InOwner);
	
private:
	UPROPERTY(EditDefaultsOnly, meta=(BindWidget))
	TObjectPtr<UTextBlock> Txt_Title;
	UPROPERTY(EditDefaultsOnly, meta=(BindWidget))
	TObjectPtr<UTextBlock> Txt_Desc;
	UPROPERTY(EditDefaultsOnly, meta=(BindWidget))
	TObjectPtr<UButton> Btn_Join;
	UPROPERTY(EditDefaultsOnly, meta=(BindWidget))
	TObjectPtr<UButton> Btn_Exit;
	UPROPERTY(EditDefaultsOnly, meta=(BindWidget))
	TObjectPtr<UButton> Btn_Exit_NotUseJoin;
	UPROPERTY(EditDefaultsOnly, meta=(BindWidget))
	TObjectPtr<USizeBox> SB_Join;
	UPROPERTY(EditDefaultsOnly, meta=(BindWidget))
	TObjectPtr<USizeBox> SB_Exit;
	UPROPERTY(EditDefaultsOnly, meta=(BindWidget))
	TObjectPtr<USizeBox> SB_Exit_NotUseJoin;

	FCContinentData CurrentContinentData; // 현재 대륙 데이터 저장

	UPROPERTY()
	TObjectPtr<ACContinentWidgetActor> OuterOwner;

	virtual void NativeConstruct() override;
	
	UFUNCTION()
	void OnJoinButtonClicked();
	UFUNCTION()
	void OnExitButtonClicked();
};
