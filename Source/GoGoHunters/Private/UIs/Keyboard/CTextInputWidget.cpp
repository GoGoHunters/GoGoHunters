#include "UIs/Keyboard/CTextInputWidget.h"
#include "Components/EditableText.h"
#include "Internationalization/Text.h"
#include "Math/UnrealMathUtility.h"
#include "Framework/Application/SlateApplication.h"

void UCTextInputWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Txt_Initial)
	{
		Txt_Initial->SetIsReadOnly(false);
		Txt_Initial->SetIsPassword(false);
		Txt_Initial->SetIsEnabled(true);

		const int32 Len = Txt_Initial->GetText().ToString().Len();
		Caret = bPlaceCaretAtEndOnConstruct ? Len : FMath::Clamp(Caret, 0, Len);

		FocusInput();
	}
}

/* ================= 포커스 ================= */

void UCTextInputWidget::FocusInput()
{
	if (!Txt_Initial) return;

	if (APlayerController* PC = GetOwningPlayer())
	{
		Txt_Initial->SetUserFocus(PC);
	}
}

FString UCTextInputWidget::GetText() const
{
	return Txt_Initial ? Txt_Initial->GetText().ToString() : FString();
}

/* ================= 공개 API ================= */

void UCTextInputWidget::PressKeyLabel(const FString& Label)
{
	if (!Txt_Initial) return;

	// Space/Backspace/Left/Right/Caps/ShiftDown/ShiftUp 등
	if (HandleCommandLabel(Label))
	{
		return;
	}

	// 인쇄 가능한 문자
	TCHAR OutChar = 0;
	if (MakeOutputCharFromLabel(Label, OutChar))
	{
		InsertChar(OutChar);

		// 일반 UX: 순간 Shift는 1글자 후 해제
		if (bShift) bShift = false;
	}
}

void UCTextInputWidget::InsertString(const FString& InString)
{
	if (!Txt_Initial || InString.IsEmpty()) return;

	const FString Cur   = Txt_Initial->GetText().ToString();

	// 제한 처리
	if (bUseCharLimit)
	{
		int32 Remain = MaxChars - Cur.Len();
		if (Remain <= 0)
		{
			BroadcastLimitIfNeeded();
			return;
		}

		FString ToInsert = InString;
		if (ToInsert.Len() > Remain)
		{
			if (bTruncateOnInsert)
			{
				ToInsert = ToInsert.Left(Remain);
			}
			else
			{
				BroadcastLimitIfNeeded();
				return;
			}
		}

		// 실제 삽입
		const FString Left  = Cur.Left(Caret);
		const FString Right = Cur.Mid(Caret);
		const FString NewText = Left + ToInsert + Right;

		SetTextAndClampCaret(NewText);
		Caret += ToInsert.Len();
		return;
	}

	// 제한 OFF일 때
	const FString Left  = Cur.Left(Caret);
	const FString Right = Cur.Mid(Caret);
	const FString NewText = Left + InString + Right;

	SetTextAndClampCaret(NewText);
	Caret += InString.Len();
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
				Caret = FMath::Clamp(Caret, 0, Cur.Len());
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
	const FString Left  = Cur.Left(Caret);
	const FString Right = Cur.Mid(Caret);

	FString NewText; NewText.Reserve(Cur.Len()+1);
	NewText = Left; NewText.AppendChar(Ch); NewText += Right;

	SetTextAndClampCaret(NewText);
	Caret++;
}

void UCTextInputWidget::Backspace()
{
	if (!Txt_Initial || Caret <= 0) return;

	const FString Cur = Txt_Initial->GetText().ToString();
	const FString NewText = Cur.Left(Caret - 1) + Cur.Mid(Caret);

	SetTextAndClampCaret(NewText);
	Caret--;
}

void UCTextInputWidget::MoveCursorLeft()
{
	Caret = FMath::Max(0, Caret - 1);
}

void UCTextInputWidget::MoveCursorRight()
{
	const int32 Len = Txt_Initial ? Txt_Initial->GetText().ToString().Len() : 0;
	Caret = FMath::Min(Len, Caret + 1);
}

void UCTextInputWidget::SetTextAndClampCaret(const FString& NewText)
{
	if (!Txt_Initial) return;

	Txt_Initial->SetText(FText::FromString(NewText));
	Caret = FMath::Clamp(Caret, 0, NewText.Len());

	// 매 입력 후에도 포커스를 유지(캐럿 표시 보장)
	FocusInput();
}

/* ================= 라벨 처리/매핑 ================= */

bool UCTextInputWidget::HandleCommandLabel(const FString& Label)
{
	if (Label.Equals(TEXT("Space"), ESearchCase::IgnoreCase))     { InsertChar(TEXT(' ')); return true; }
	if (Label.Equals(TEXT("Backspace"), ESearchCase::IgnoreCase)) { Backspace();           return true; }
	if (Label.Equals(TEXT("Left"), ESearchCase::IgnoreCase))      { MoveCursorLeft();      return true; }
	if (Label.Equals(TEXT("Right"), ESearchCase::IgnoreCase))     { MoveCursorRight();     return true; }
	if (Label.Equals(TEXT("Caps"), ESearchCase::IgnoreCase))      { ToggleCaps();          return true; }
	if (Label.Equals(TEXT("ShiftDown"), ESearchCase::IgnoreCase)) { ShiftDown();           return true; }
	if (Label.Equals(TEXT("ShiftUp"), ESearchCase::IgnoreCase))   { ShiftUp();             return true; }
	return false;
}

bool UCTextInputWidget::MakeOutputCharFromLabel(const FString& Label, TCHAR& OutChar) const
{
	// 1) Numpad/이름형 기호 먼저 처리 (키패드는 Shift 영향 없음)
	if (TryMapNumpadToChar(Label, OutChar)) return true;
	if (TryMapNamedKeyToChar(Label, OutChar)) return true;

	// 2) 단일 문자 라벨
	if (Label.Len() == 1)
	{
		const TCHAR C = Label[0];

		// 알파벳: 라벨은 소문자 전달 권장
		if (C >= 'a' && C <= 'z')
		{
			const bool bUpper = bCapsLock ^ bShift; // XOR
			OutChar = bUpper ? (C - ('a' - 'A')) : C;
			return true;
		}

		// 숫자/기호(상단 숫자열 및 기본 기호)
		if ((C >= '0' && C <= '9') || FString(TEXT("`-=[]\\;',./")).Contains(FString::Chr(C)))
		{
			if (bShift)
			{
				TCHAR Shifted;
				if (MapShiftedSymbol(C, Shifted)) { OutChar = Shifted; return true; }
			}
			OutChar = C;
			return true;
		}
	}
	return false;
}

bool UCTextInputWidget::TryMapNamedKeyToChar(const FString& Label, TCHAR& OutChar) const
{
	const FString L = Label.ToLower();

	// 이름형 기호 라벨 → 문자
	if (L == "minus"     || L == "hyphen")         { OutChar = '-'; return true; }
	if (L == "equals"    || L == "equal")          { OutChar = '='; return true; }
	if (L == "period"    || L == "dot")            { OutChar = '.'; return true; }
	if (L == "comma")                               { OutChar = ','; return true; }
	if (L == "slash"     || L == "forwardslash")    { OutChar = '/'; return true; }
	if (L == "backslash")                           { OutChar = '\\'; return true; }
	if (L == "semicolon")                           { OutChar = ';'; return true; }
	if (L == "apostrophe"|| L == "quote")           { OutChar = '\''; return true; }
	if (L == "grave"     || L == "backtick")        { OutChar = '`'; return true; }
	if (L == "leftbracket"  || L == "lbracket")     { OutChar = '['; return true; }
	if (L == "rightbracket" || L == "rbracket")     { OutChar = ']'; return true; }

	// 선택적으로 기호 직결
	if (L == "plus")                                { OutChar = '+'; return true; }
	if (L == "underscore")                          { OutChar = '_'; return true; }
	if (L == "colon")                               { OutChar = ':'; return true; }
	if (L == "question")                            { OutChar = '?'; return true; }
	if (L == "pipe")                                { OutChar = '|'; return true; }
	if (L == "tilde")                               { OutChar = '~'; return true; }
	if (L == "braceleft"  || L == "lbrace")         { OutChar = '{'; return true; }
	if (L == "braceright" || L == "rbrace")         { OutChar = '}'; return true; }
	if (L == "dblquote"   || L == "doublequote")    { OutChar = '"'; return true; }
	if (L == "less")                                { OutChar = '<'; return true; }
	if (L == "greater")                             { OutChar = '>'; return true; }

	return false;
}

bool UCTextInputWidget::TryMapNumpadToChar(const FString& Label, TCHAR& OutChar) const
{
	// "Numpad0..9", "Num0..9", "KP_0..9", 연산(Add/Sub/Mul/Div/Decimal)
	const FString L = Label.ToLower();

	auto LastDigit = [&]() -> int32
	{
		for (int32 i = L.Len()-1; i >= 0; --i)
		{
			if (FChar::IsDigit(L[i])) return (int32)(L[i]-'0');
		}
		return -1;
	};

	if (L.StartsWith(TEXT("numpad")) || L.StartsWith(TEXT("num")) || L.StartsWith(TEXT("kp_")))
	{
		const int32 D = LastDigit();
		if (0 <= D && D <= 9) { OutChar = TCHAR('0' + D); return true; }

		// 연산/소수점
		if (L.Contains(TEXT("add"))      || L.EndsWith(TEXT("+"))) { OutChar = '+'; return true; }
		if (L.Contains(TEXT("sub"))      || L.EndsWith(TEXT("-"))) { OutChar = '-'; return true; }
		if (L.Contains(TEXT("mul"))      || L.Contains(TEXT("times")) || L.EndsWith(TEXT("*"))) { OutChar = '*'; return true; }
		if (L.Contains(TEXT("div"))      || L.EndsWith(TEXT("/"))) { OutChar = '/'; return true; }
		if (L.Contains(TEXT("decimal"))  || L.EndsWith(TEXT("."))) { OutChar = '.'; return true; }
	}
	return false;
}

bool UCTextInputWidget::MapShiftedSymbol(TCHAR Base, TCHAR& OutShifted) const
{
	// US 키보드 상단 숫자열 Shift 매핑
	switch (Base)
	{
	case '`': OutShifted = '~'; return true;
	case '1': OutShifted = '!'; return true;
	case '2': OutShifted = '@'; return true;
	case '3': OutShifted = '#'; return true;
	case '4': OutShifted = '$'; return true;
	case '5': OutShifted = '%'; return true;
	case '6': OutShifted = '^'; return true;
	case '7': OutShifted = '&'; return true;
	case '8': OutShifted = '*'; return true;
	case '9': OutShifted = '('; return true;
	case '0': OutShifted = ')'; return true;
	case '-': OutShifted = '_'; return true;
	case '=': OutShifted = '+'; return true;
	case '[': OutShifted = '{'; return true;
	case ']': OutShifted = '}'; return true;
	case '\\':OutShifted = '|'; return true;
	case ';': OutShifted = ':'; return true;
	case '\'':OutShifted = '"'; return true;
	case ',': OutShifted = '<'; return true;
	case '.': OutShifted = '>'; return true;
	case '/': OutShifted = '?'; return true;
	default:  return false;
	}
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
