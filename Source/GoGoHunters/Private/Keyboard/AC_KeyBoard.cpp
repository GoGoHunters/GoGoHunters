// Fill out your copyright notice in the Description page of Project Settings.


#include "KeyBoard/AC_KeyBoard.h"

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
}

// Called when the game starts or when spawned
void AAC_KeyBoard::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAC_KeyBoard::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	HandleKeyClicked("test");
}

void AAC_KeyBoard::GenerateKeys()
{
	// 기존에 생성된 키들이 있다면 모두 삭제
	for (AAC_Key* Key : Keys)
	{
		if (Key)
			Key->Destroy();
	}

	Keys.Empty();

	UWorld* World = GetWorld();

	float MaxWidth = 0.0f;
	float TotalHeight = -KeySpacing;

	if (!KeyLayoutData.IsValidIndex(KeyLayoutIndex))
	{
		UE_LOG(LogTemp, Display, TEXT("No Keyboard Layout for Index : %d"), KeyLayoutIndex);
		return; 
	}

	const FKeyBoardLayOutData& KeyLayout = KeyLayoutData[KeyLayoutIndex];


	for (const FKeyBoardRowData& RowData : KeyLayout.KeyLayOutArray)
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
		float YOffset = (TotalHeight - KeyHeight)* -0.5f;
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

				AAC_Key* NewKey = World->SpawnActor<AAC_Key>(GetActorLocation(), GetActorRotation(), SpawnParams);

				if (NewKey)
				{
					NewKey->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);

					NewKey->SetActorRelativeLocation(SpawnRelativeLocation);

					NewKey->SetKeyText(KeyData.KeyChar);
					NewKey->Size_X = KeyData.KeySizeX;
					NewKey->Size_Y = this->KeyHeight;
					NewKey->Size_Z = 8.0f;

					NewKey->UpdateMeshScale();

					NewKey->OnkeyClicked.AddDynamic(this, &AAC_KeyBoard::HandleKeyClicked);

					Keys.Add(NewKey);
				}

				XOffset += KeyData.KeySizeX + KeySpacing;
			}
			YOffset += this->KeyHeight + KeySpacing;
		}
	}
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

	KeyLayoutIndex = CapsOn ? KeyLayoutBaseIndex + 1 : KeyLayoutBaseIndex;

	if (KeyString.Equals(TEXT("CAPS"), ESearchCase::IgnoreCase))
	{
		CapsOn = CapsOn ? 0 : 1;
	}
	else if (KeyString.Equals(TEXT("SHIFT"), ESearchCase::IgnoreCase) && KeyLayoutIndex != 1)
	{
		KeyLayoutIndex = CapsOn ? KeyLayoutBaseIndex : KeyLayoutBaseIndex + 1;
	}

	if (OnkeyBoardClicked.IsBound())
	{
		OnkeyBoardClicked.Broadcast(KeyString);
	}

	GenerateKeys();
}