// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AC_Key.h"
#include "AC_KeyBoard.generated.h"


class UWidgetComponent;
class UCTextInputWidget;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnKeyBoardClickedEvent, FString, KeyString);

USTRUCT(BlueprintType)
struct FKeyBoardRowData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Keyboard")
	TArray<FKeyData> KeyDataArray;
};

USTRUCT(BlueprintType)
struct FKeyBoardLayOutData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Keyboard")
	TArray<FKeyBoardRowData> KeyLayOutArray;
};

USTRUCT(BlueprintType)
struct FKeyDataLayer
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Keyboard")
	TArray<AAC_Key*> Keys;
};

UCLASS()
class GOGOHUNTERS_API AAC_KeyBoard : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	AAC_KeyBoard();
	
	// virtual void OnConstruction(const FTransform& Transform) override;

private: 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* RootSceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* KeyBoardMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* KeyBoardInputWidgetComp;

	float BaseSize_X = 500.0f;
	float BaseSize_Y = 200.0f;
	float BaseSize_Z = 8.0f;

	void UpdateBaseMeshScale();

	UFUNCTION()
	void HandleKeyClicked(FString KeyString);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, Category = "Keyboard")
	TArray<FKeyDataLayer> KeysLayer;

public:
	UPROPERTY(EditAnywhere, Category = "Keyboard")
	TArray<FKeyBoardLayOutData> KeyLayoutData;


	UPROPERTY(EditAnywhere, Category = "Keyboard")
	bool CapsOn = 0;

	UPROPERTY(EditAnywhere, Category = "Keyboard")
	int KeyLayoutIndex = 0;

	UPROPERTY(EditAnywhere, Category = "Keyboard")
	int KeyLayoutBaseIndex = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UStaticMesh* KeyBoard_Mesh;

	UFUNCTION(BlueprintCallable, Category = "Keyboard")
	void GenerateKeys();

	UPROPERTY(EditAnywhere, Category = "Keyboard")
	float KeyHeight = 19.0f;

	UPROPERTY(EditAnywhere, Category = "Keyboard")
	float KeySpacing = 1.4f;


	UPROPERTY(EditAnywhere, Category = "Keyboard")
	float KeyBoardPadding_X = 20.0f;
	UPROPERTY(EditAnywhere, Category = "Keyboard")
	float KeyBoardPadding_Y = 12.0f;


	UPROPERTY(BlueprintAssignable, Category = "Keyboard")
	FOnKeyBoardClickedEvent OnkeyBoardClicked;

	UFUNCTION()
	void UpdateKeybaordSize();

	UFUNCTION(BlueprintCallable, Category = "Keyboard")
	void UpdateVisibleLayer(int index);

private:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCTextInputWidget> KeyBoardInputWidget;
};
