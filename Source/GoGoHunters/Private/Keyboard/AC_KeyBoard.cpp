// Fill out your copyright notice in the Description page of Project Settings.


#include "KeyBoard/AC_KeyBoard.h"

#include "Components/WidgetComponent.h"
#include "UIs/Keyboard/CAlertMsgWidget.h"
#include "UIs/Keyboard/CTextInputWidget.h"
#include "Utilities/CHelpers.h"

// Sets default values
AAC_KeyBoard::AAC_KeyBoard()
{

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Game/KeyBoard/assets/Cube_KeyBoard.Cube_KeyBoard"));

	if (MeshAsset.Succeeded())
	{
		// This is not a component, so you'd likely want to store this in a UPROPERTY.
		KeyBoard_Mesh = MeshAsset.Object;
	}

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	this->SetRootComponent(RootSceneComponent);

	KeyBoardMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("KeyMesh"));
	KeyBoardMeshComponent->SetupAttachment(RootSceneComponent);

	if (KeyBoard_Mesh)
	{
		KeyBoardMeshComponent->SetStaticMesh(KeyBoard_Mesh);

		FVector MeshSize_cm = KeyBoard_Mesh->GetBoundingBox().GetExtent() * 2;
		KeyBoardMeshComponent->SetRelativeScale3D(FVector(BaseSize_X / MeshSize_cm.X, BaseSize_Y / MeshSize_cm.Y, BaseSize_Z / MeshSize_cm.Z));
	}

	CHelpers::CreateComponent<UWidgetComponent>(this, &KeyBoardInputWidgetComp, "KeyBoardInputWidget", RootComponent);
	CHelpers::CreateComponent<UWidgetComponent>(this, &AlertMsgWidgetComp, "AlertMsgWidgetComp", RootComponent);
	AlertMsgWidgetComp->SetVisibility(false);
}

// Called when the game starts or when spawned
void AAC_KeyBoard::BeginPlay()
{
	Super::BeginPlay();

	GenerateKeys();

	if (UUserWidget* widget = KeyBoardInputWidgetComp->GetWidget())
	{
		KeyBoardInputWidget = Cast<UCTextInputWidget>(widget);
		if (KeyBoardInputWidget)
			KeyBoardInputWidget->SetKeyboard(this);
	}
	if (UUserWidget* widget = AlertMsgWidgetComp->GetWidget())
	{
		AlertMsgWidget = Cast<UCAlertMsgWidget>(widget);
		if (AlertMsgWidget)
			AlertMsgWidget->SetKeyboard(this);
	}
}

/*
void AAC_KeyBoard::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	HandleKeyClicked("test");
}
*/

void AAC_KeyBoard::GenerateKeys()
{
	// ������ ������ Ű���� �ִٸ� ��� ����
	for (FKeyDataLayer KeyArray : KeysLayer)
	{
		for (AAC_Key* Key : KeyArray.Keys)
		{
			if (Key)
				Key->Destroy();
		}
		KeyArray.Keys.Empty();
	}
	KeysLayer.Empty();

	UWorld* World = GetWorld();

	if (!KeyLayoutData.IsValidIndex(KeyLayoutIndex))
	{
		UE_LOG(LogTemp, Display, TEXT("No Keyboard Layout for Index : %d"), KeyLayoutIndex);
		return; 
	}

	float MaxWidth = 0.0f;
	float TotalHeight = -KeySpacing;

	const FKeyBoardLayOutData& TargetKeyLayout = KeyLayoutData[KeyLayoutIndex];
	for (const FKeyBoardRowData& RowData : TargetKeyLayout.KeyLayOutArray)
	{
		float RowWidth = 0.0f;
		for (const FKeyData& KeyData : RowData.KeyDataArray)
		{
			RowWidth += KeyData.KeySizeX + KeySpacing;
		}
		MaxWidth = FMath::Max(MaxWidth, RowWidth);
		TotalHeight += this->KeyHeight + KeySpacing;
	}
	BaseSize_X = MaxWidth;
	BaseSize_Y = TotalHeight;

	UpdateBaseMeshScale();

	if (World)
	{
		int32 Index = 0;
		for (const FKeyBoardLayOutData& KeyLayout : KeyLayoutData)
		{
			float YOffset = (TotalHeight - KeyHeight)* -0.5f;
			FKeyDataLayer KeyArray;
			for (const FKeyBoardRowData& RowData : KeyLayout.KeyLayOutArray)
			{
				float XOffset = MaxWidth * -0.5f;
				
				for (const FKeyData& KeyData : RowData.KeyDataArray)
				{
					FActorSpawnParameters SpawnParams;
					SpawnParams.Owner = this;
					SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

					FVector SpawnRelativeLocation = FVector(XOffset + KeyData.KeySizeX * 0.5f , YOffset, BaseSize_Z * 0.75f);
					FRotator SpawnRotation = FRotator::ZeroRotator;

					AAC_Key* NewKey = World->SpawnActor<AAC_Key>(SpawnRelativeLocation, SpawnRotation, SpawnParams);

					if (NewKey)
					{
						NewKey->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);

						NewKey->SetActorRelativeLocation(SpawnRelativeLocation);

						NewKey->SetKeyText(KeyData.KeyChar);
						NewKey->Size_X = KeyData.KeySizeX;
						NewKey->Size_Y = this->KeyHeight;
						NewKey->Size_Z = 8.0f;
					
						NewKey->UpdateMeshScale();
						UE_LOG(LogTemp, Warning, TEXT("Binding delegate for key: %s"), *KeyData.KeyChar);
						NewKey->OnkeyClicked.AddDynamic(this, &AAC_KeyBoard::HandleKeyClicked);

						UE_LOG(LogTemp, Warning, TEXT("Delegate bound successfully"));
						
						if (Index != KeyLayoutIndex)
						{
							NewKey->SetActorEnableCollision(false); 
							NewKey->SetActorHiddenInGame(true);
						}

						KeyArray.Keys.Add(NewKey);
					}

					XOffset += KeyData.KeySizeX + KeySpacing;
				}
				YOffset += this->KeyHeight + KeySpacing;
			}
			KeysLayer.Add(KeyArray);
			Index++;
		}
	}
}

void AAC_KeyBoard::UpdateKeybaordSize()
{
	float MaxWidth = 0.0f;
	float TotalHeight = -KeySpacing;

	const FKeyBoardLayOutData& TargetKeyLayout = KeyLayoutData[KeyLayoutIndex];
	for (const FKeyBoardRowData& RowData : TargetKeyLayout.KeyLayOutArray)
	{
		float RowWidth = 0.0f;
		for (const FKeyData& KeyData : RowData.KeyDataArray)
		{
			RowWidth += KeyData.KeySizeX + KeySpacing;
		}
		MaxWidth = FMath::Max(MaxWidth, RowWidth);
		TotalHeight += this->KeyHeight + KeySpacing;
	}
	BaseSize_X = MaxWidth;
	BaseSize_Y = TotalHeight;

	UpdateBaseMeshScale();
}

void AAC_KeyBoard::UpdateVisibleLayer(int index)
{
	for (AAC_Key* key: KeysLayer[KeyLayoutIndex].Keys)
	{
		key->SetActorEnableCollision(true);
		key->SetActorHiddenInGame(false);
	}
	KeyLayoutIndex = index;
	for (AAC_Key* key : KeysLayer[KeyLayoutIndex].Keys)
	{
		key->SetActorEnableCollision(false);
		key->SetActorHiddenInGame(true);
	}
	UpdateKeybaordSize();
}

void AAC_KeyBoard::EnterPlayerInitial(const FString& Initial)
{
	// GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("%s"), *Initial));
	AlertMsgWidgetComp->SetVisibility(true);
	AlertMsgWidget->SetInitialText(Initial);	
}

void AAC_KeyBoard::FinishEnterPlayerInitial()
{
	AlertMsgWidgetComp->SetVisibility(false);
	KeyBoardInputWidget->CompleteRequest();
}

void AAC_KeyBoard::UpdateBaseMeshScale()
{
	if (KeyBoardMeshComponent && KeyBoard_Mesh)
	{
		FVector MeshSize_cm = KeyBoard_Mesh->GetBoundingBox().GetExtent() * 2;

		FVector NewScale = FVector(
			(this->BaseSize_X + KeyBoardPadding_X) / MeshSize_cm.X, 
			(this->BaseSize_Y + KeyBoardPadding_Y) / MeshSize_cm.Y, 
			this->BaseSize_Z / MeshSize_cm.Z
		);

		KeyBoardMeshComponent->SetRelativeScale3D(NewScale);
	}
}

void AAC_KeyBoard::HandleKeyClicked(FString KeyString)
{
	UE_LOG(LogTemp, Warning, TEXT("Key Clicked: %s"), *KeyString);

	if (OnkeyBoardClicked.IsBound())
	{
		OnkeyBoardClicked.Broadcast(KeyString);
	}


	if (KeyString.Equals(TEXT("CAPS"), ESearchCase::IgnoreCase))
	{
		CapsOn = CapsOn ? 0 : 1;
		UpdateVisibleLayer(KeyLayoutBaseIndex + CapsOn);
	}
	else if (KeyString.Equals(TEXT("SHIFT"), ESearchCase::IgnoreCase) && KeyLayoutIndex != 1)
	{
		CapsOn ? KeyLayoutBaseIndex : KeyLayoutBaseIndex + 1;
		UpdateVisibleLayer(CapsOn ? KeyLayoutBaseIndex : KeyLayoutBaseIndex + 1);
	}
}