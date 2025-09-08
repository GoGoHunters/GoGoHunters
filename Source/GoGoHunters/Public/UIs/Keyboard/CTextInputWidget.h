#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CTextInputWidget.generated.h"

class UEditableText;

/** 문자 제한에 걸렸을 때 브로드캐스트 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharLimitReached);

/**
 * 
 */
UCLASS()
class GOGOHUNTERS_API UCTextInputWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** 키 라벨 기반 입력 (a, 1, -, Space, Backspace, Left, Right, Caps, ShiftDown, ShiftUp, Numpad0..9 등) */
	UFUNCTION(BlueprintCallable, Category="VirtualKeyboard")
	void PressKeyLabel(const FString& Label);

	/** 문자열 통째로 삽입 (붙여넣기 등) */
	UFUNCTION(BlueprintCallable, Category="VirtualKeyboard")
	void InsertString(const FString& InString);

	/** 위젯에 UI 포커스를 줘서 캐럿이 확실히 보이게 */
	UFUNCTION(BlueprintCallable, Category="VirtualKeyboard")
	void FocusInput();

	/** 상태 조회 */
	UFUNCTION(BlueprintPure, Category="VirtualKeyboard")
	int32 GetCaret() const { return Caret; }

	UFUNCTION(BlueprintPure, Category="VirtualKeyboard")
	FString GetText() const;

	/** 제한에 걸렸을 때 알림(BP에서 텍스트 빨갛게, 진동/SFX 등) */
	UPROPERTY(BlueprintAssignable, Category="VirtualKeyboard|CharLimit")
	FOnCharLimitReached OnCharLimitReached;
	
private:
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UEditableText* Txt_Initial;

	/** 생성 시 커서를 끝으로 둘지 여부 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="VirtualKeyboard", meta = (AllowPrivateAccess=true))
	bool bPlaceCaretAtEndOnConstruct = true;

	// 편집 상태
	int32 Caret = 0;
	bool bCapsLock = false;
	bool bShift = false;

	/** 제한 초과 문자열 삽입 시: true=남은 분량만 잘라서 삽입, false=전체 거부 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="VirtualKeyboard|CharLimit", meta = (AllowPrivateAccess=true))
	bool bTruncateOnInsert = true;
	// 문자 제한 상태
	bool  bUseCharLimit = false;
	int32 MaxChars = 20; // bUseCharLimit=true일 때만 의미 있음

	virtual void NativeConstruct() override;
	
	// 핵심 동작
	void InsertChar(TCHAR Ch);
	void Backspace();
	void MoveCursorLeft();
	void MoveCursorRight();
	void ToggleCaps() { bCapsLock = !bCapsLock; }
	void ShiftDown()  { bShift = true;  }
	void ShiftUp()    { bShift = false; }

	void SetTextAndClampCaret(const FString& NewText);

	// 라벨 → 문자 변환
	bool MakeOutputCharFromLabel(const FString& Label, TCHAR& OutChar) const;
	bool TryMapNamedKeyToChar(const FString& Label, TCHAR& OutChar) const;  // Minus/Equals/Period/...
	bool TryMapNumpadToChar(const FString& Label, TCHAR& OutChar) const;    // Numpad0..9, Add/Sub/Mul/Div/Decimal
	bool MapShiftedSymbol(TCHAR Base, TCHAR& OutShifted) const;             // 상단 숫자열 Shift 기호

	// 명령 라벨 처리
	bool HandleCommandLabel(const FString& Label);

	/** ===== 문자 수 제한 제어 ===== */

	/** 문자 제한 켜기/끄기 */
	UFUNCTION(BlueprintCallable, Category="VirtualKeyboard|CharLimit")
	void SetCharLimitEnabled(bool bEnable);

	/** 최대 글자 수 설정. bEnable=true면 즉시 제한 적용, bTrimIfOver=true면 현재 텍스트가 초과 시 잘라냄 */
	UFUNCTION(BlueprintCallable, Category="VirtualKeyboard|CharLimit")
	void SetCharLimit(int32 InMax, bool bEnable = true, bool bTrimIfOver = false);

	/** 현재 제한 정보 */
	UFUNCTION(BlueprintPure, Category="VirtualKeyboard|CharLimit")
	bool IsCharLimitEnabled() const { return bUseCharLimit; }

	UFUNCTION(BlueprintPure, Category="VirtualKeyboard|CharLimit")
	int32 GetMaxChars() const { return MaxChars; }

	/** 남은 글자 수(제한이 꺼져 있으면 큰 값 반환) */
	UFUNCTION(BlueprintPure, Category="VirtualKeyboard|CharLimit")
	int32 GetRemainingChars() const;

	// 제한 체크 유틸
	bool CanInsertCount(int32 CountToAdd) const;
	int32 RemainingForInsert() const;
	void BroadcastLimitIfNeeded() const;
};
