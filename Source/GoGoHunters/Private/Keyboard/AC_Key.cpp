// Fill out your copyright notice in the Description page of Project Settings.


#include "Keyboard/AC_Key.h"

// Sets default values
AAC_Key::AAC_Key()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Game/KeyBoard/assets/Cube_Key.Cube_Key"));

	if (MeshAsset.Succeeded())
		Key_Mesh = MeshAsset.Object;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	this->SetRootComponent(RootSceneComponent);

	KeySceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("KeyScene"));
	KeySceneComponent->SetupAttachment(RootSceneComponent);

	KeyMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("KeyMesh"));
	KeyMeshComponent->AttachToComponent(KeySceneComponent, FAttachmentTransformRules::KeepRelativeTransform);
	//KeyMeshComponent->SetCollisionEnabled 추후 콜리전 설정

	KeyTextComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("KeyText"));
	KeyTextComponent->AttachToComponent(KeySceneComponent, FAttachmentTransformRules::KeepRelativeTransform);
		
	KeyTextComponent->SetRelativeRotation(FRotator(90.0f, 90.0f, 0.0f));
	KeyTextComponent->SetRelativeScale3D(FVector(0.05f));
	KeyTextComponent->SetHorizontalAlignment(EHTA_Center);
	KeyTextComponent->SetVerticalAlignment(EVRTA_TextCenter);
	KeyTextComponent->SetWorldSize(192.0f);

	if (Key_Mesh)
	{
		KeyMeshComponent->SetStaticMesh(Key_Mesh);

		FVector MeshSize_cm = Key_Mesh->GetBoundingBox().GetExtent() * 2;

		KeyMeshComponent->SetRelativeScale3D(FVector(Size_X / MeshSize_cm.X, Size_Y / MeshSize_cm.Y, Size_Z / MeshSize_cm.Z));
		KeyTextComponent->SetRelativeLocation(FVector(0.0f, 0.0f, (Size_Z * 0.5f) + 0.01f));
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> BaseMaterialAsset(TEXT("/Game/KeyBoard/assets/M_Key_Base.M_Key_Base"));
	if (BaseMaterialAsset.Succeeded())
	{
		BaseMaterial = BaseMaterialAsset.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> HoverMaterialAsset(TEXT("/Game/KeyBoard/assets/M_Key_Hover.M_Key_Hover"));
	if (HoverMaterialAsset.Succeeded())
	{
		HoverMaterial = HoverMaterialAsset.Object;
	}
}

// Called when the game starts or when spawned
void AAC_Key::BeginPlay()
{
	Super::BeginPlay();

}

void AAC_Key::SetKeyText(const FString& NewText)
{
	if (KeyTextComponent)
	{
		KeyTextComponent->SetText(FText::FromString(NewText));
	}
}

void AAC_Key::SetKeyMaterial(UMaterialInterface* NewMaterial)
{
	if (KeyMeshComponent && NewMaterial)
	{
		KeyMeshComponent->SetMaterial(0, NewMaterial);
	}
}


void AAC_Key::SetKeyTextColor(FColor NewColor)
{
	if (KeyTextComponent)
	{
		KeyTextComponent->SetTextRenderColor(NewColor);
	}
}

void AAC_Key::SetHover(bool bIsHovered)
{
	if (bIsHovered)
	{
		SetKeyMaterial(HoverMaterial);
		SetKeyTextColor(HoverTextColor);
	}
	else
	{
		SetKeyMaterial(BaseMaterial);
		SetKeyTextColor(BaseTextColor);
		KeyRelease();
	}
}

void AAC_Key::KeyPress()
{
	this->KeySceneComponent->SetRelativeLocation(FVector(0, 0, key_press_move));

	if (OnkeyClicked.IsBound())
	{
		OnkeyClicked.Broadcast(this->key_word);
	}
}

void AAC_Key::KeyRelease()
{
	this->KeySceneComponent->SetRelativeLocation(FVector::ZeroVector);
}

void AAC_Key::UpdateMeshScale()	
{
	if (Key_Mesh && KeyMeshComponent)
	{
		FVector MeshSize_cm = Key_Mesh->GetBoundingBox().GetExtent() * 2;
		FVector NewScale = FVector(Size_X / MeshSize_cm.X, Size_Y / MeshSize_cm.Y, Size_Z / MeshSize_cm.Z);
		KeyMeshComponent->SetRelativeScale3D(NewScale);
		KeyTextComponent->SetRelativeLocation(FVector(0.0f, 0.0f, (Size_Z * 0.5f) + 0.01f));
	}
}