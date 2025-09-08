#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CTextInputWidget.generated.h"

class UEditableText;
/**
 * 
 */
UCLASS()
class GOGOHUNTERS_API UCTextInputWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UEditableText* Txt_Initial;
};
