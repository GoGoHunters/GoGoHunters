#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CContinentWidget.generated.h"

class USizeBox;
class UTextBlock;
class UButton;
struct FCContinentData;

/**
 * 
 */
UCLASS()
class GOGOHUNTERS_API UCContinentWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetContinentData(const FCContinentData& ContinentData);

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
	TObjectPtr<USizeBox> SB_Join;
	UPROPERTY(EditDefaultsOnly, meta=(BindWidget))
	TObjectPtr<USizeBox> SB_Exit;
	UPROPERTY(EditDefaultsOnly, meta=(BindWidget))
	TObjectPtr<USizeBox> SB_Exit_NotUseJoin;
};
