// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h" 
#include "AC_Key.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnKeyClickedEvent, FString, KeyString);


USTRUCT(BlueprintType)
struct FKeyData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Key")
	FString KeyChar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Key")
	float KeySizeX = 24.0f;
};

UCLASS()
class GOGOHUNTERS_API AAC_Key : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	AAC_Key();

private:
	UPROPERTY()
	float key_press_move = -4.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* RootSceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* KeySceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* KeyMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UTextRenderComponent* KeyTextComponent;


	void SetKeyMaterial(UMaterialInterface* NewMaterial);

	void SetKeyTextColor(FColor NewColor);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Key")
	FString key_word;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Size_X = 32.0f;

	float Size_Y = 19.0f;
	float Size_Z = 5.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UStaticMesh* Key_Mesh;

	// 키 버튼
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Materials")
	UMaterialInterface* BaseMaterial;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Materials")
	UMaterialInterface* HoverMaterial;

	// 키 텍스트
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Key")
	FColor  BaseTextColor = FColor::White;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Key")
	FColor  HoverTextColor = FColor::Black;


	UFUNCTION(BlueprintCallable, Category = "Key")
	void SetKeyText(const FString& NewText);

	UFUNCTION(BlueprintCallable, Category = "Key")
	void SetHover(bool bIsHovered);

	UFUNCTION(BlueprintCallable, Category = "Key")
	void KeyPress();

	UFUNCTION(BlueprintCallable, Category = "Key")
	void KeyRelease();

	UPROPERTY(BlueprintAssignable, Category = "AudioRecording")
	FOnKeyClickedEvent OnkeyClicked;

	UFUNCTION(BlueprintCallable, Category = "Key")
	void UpdateMeshScale();
};
