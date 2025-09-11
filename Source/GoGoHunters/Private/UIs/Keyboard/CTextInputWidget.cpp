#include "UIs/Keyboard/CTextInputWidget.h"
#include "Components/EditableText.h"
#include "Internationalization/Text.h"
#include "Math/UnrealMathUtility.h"
#include "Framework/Application/SlateApplication.h"
#include "GameFramework/PlayerController.h"
#include "Engine/EngineTypes.h"
#include "InputCoreTypes.h"
#include "Keyboard/AC_KeyBoard.h"

void UCTextInputWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Txt_Initial)
	{
		Txt_Initial->SetIsReadOnly(false);
		Txt_Initial->SetIsPassword(false);
		Txt_Initial->SetIsEnabled(true);

		const int32 Len = Txt_Initial->GetText().ToString().Len();
	}
}

/* ================= 텍스트 반환 ================= */
FString UCTextInputWidget::GetText() const
{
	return Txt_Initial ? Txt_Initial->GetText().ToString() : FString();
}

/* ================= 공개 API ================= */

void UCTextInputWidget::PressKeyLabel(const FString& Label)
{
	if (!Txt_Initial) return;
	if (bRequest) return;

	// Space/BackSpace/Shift/Enter
	if (HandleCommandLabel(Label))
	{
		return;
	}

	// 인쇄 가능한 문자
	TCHAR OutChar = 0;
	if (MakeOutputCharFromLabel(Label, OutChar))
	{
		InsertChar(OutChar);
	}
}

void UCTextInputWidget::SetCharLimitEnabled(bool bEnable)
{
	bUseCharLimit = bEnable;

	// 껐다 켰을 때 현재 길이가 초과인데도 켠 경우, 잘라낼지는 사용자가 SetCharLimit()에서 제어
}

void UCTextInputWidget::SetCharLimit(int32 InMax, bool bEnable, bool bTrimIfOver)
{
	MaxChars = FMath::Max(0, InMax);
	bUseCharLimit = bEnable;

	if (!Txt_Initial) return;

	if (bUseCharLimit)
	{
		FString Cur = Txt_Initial->GetText().ToString();
		if (Cur.Len() > MaxChars)
		{
			if (bTrimIfOver)
			{
				Cur = Cur.Left(MaxChars);
				Txt_Initial->SetText(FText::FromString(Cur));
			}
			else
			{
				// 유지하되, 이후 삽입만 막음
				BroadcastLimitIfNeeded();
			}
		}
	}
}

int32 UCTextInputWidget::GetRemainingChars() const
{
	if (!bUseCharLimit || !Txt_Initial) return INT_MAX;
	const int32 CurLen = Txt_Initial->GetText().ToString().Len();
	return FMath::Max(0, MaxChars - CurLen);
}

/* ================= 내부 편집 동작 ================= */

void UCTextInputWidget::InsertChar(TCHAR Ch)
{
	if (!Txt_Initial) return;

	// 제한 검사(1글자)
	if (bUseCharLimit && !CanInsertCount(1))
	{
		BroadcastLimitIfNeeded();
		return;
	}

	const FString Cur   = Txt_Initial->GetText().ToString();

	FString NewText; NewText.Reserve(Cur.Len()+1);
	NewText = Cur;
	NewText.AppendChar(Ch);

	SetTextInTextBox(NewText);
}

void UCTextInputWidget::Backspace()
{
	if (!Txt_Initial) return;

	const FString Cur = Txt_Initial->GetText().ToString();
	if (Cur.IsEmpty())
	{
		return;
	}
	const FString NewText = Cur.Left(Cur.Len() - 1);

	SetTextInTextBox(NewText);
}

void UCTextInputWidget::SetTextInTextBox(const FString& NewText)
{
	if (!Txt_Initial) return;

	Txt_Initial->SetText(FText::FromString(NewText));
}

/* ================= 라벨 처리/매핑 ================= */

bool UCTextInputWidget::HandleCommandLabel(const FString& Label)
{
	if (Label.Equals(TEXT("Space"), ESearchCase::IgnoreCase))     	{ InsertChar(TEXT(' ')); return true; }
	if (Label.Equals(TEXT("BackSpace"), ESearchCase::IgnoreCase)) 	{ Backspace();           return true; }
	// Shift 입력은 무시	
	if (Label.Equals(TEXT("Shift"), ESearchCase::IgnoreCase))     	{                        return true; }
	if (Label.Equals(TEXT("Caps"), ESearchCase::IgnoreCase))     	{                        return true; }
	//
	if (Label.Equals(TEXT("Enter"), ESearchCase::IgnoreCase))		{ KeyBoard->EnterPlayerInitial(GetText()); bRequest = true; return true; }
	return false;
}

bool UCTextInputWidget::MakeOutputCharFromLabel(const FString& Label, TCHAR& OutChar) const
{
	// 단일 문자 라벨은 그대로 출력
	if (Label.Len() == 1)
	{
		OutChar = Label[0];
		return true;
	}
	return false;
}

/* ================= 제한 유틸 ================= */

bool UCTextInputWidget::CanInsertCount(int32 CountToAdd) const
{
	if (!bUseCharLimit || !Txt_Initial) return true;
	const int32 CurLen = Txt_Initial->GetText().ToString().Len();
	return (CurLen + CountToAdd) <= MaxChars;
}

int32 UCTextInputWidget::RemainingForInsert() const
{
	return GetRemainingChars();
}

void UCTextInputWidget::BroadcastLimitIfNeeded() const
{
	if (bUseCharLimit)
	{
		// BP에서 바인드해 시각/청각 피드백 주면 UX 좋음
		const_cast<UCTextInputWidget*>(this)->OnCharLimitReached.Broadcast();
	}
}
