#include "JMH/CMuseumComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "MotionControllerComponent.h"
#include "Components/WidgetInteractionComponent.h"
#include "JMH/MH_VRPlayer.h"
#include "LHJ/CMuseumActorBase.h"
#include "LHJ/CRelicBase.h"
#include "Utilities/CHelpers.h"
#include "base/GI_Base.h"

UCMuseumComponent::UCMuseumComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCMuseumComponent::BeginPlay()
{
	Super::BeginPlay();
	OwnerPlayer = Cast<AMH_VRPlayer>(GetOwner());

	if (APlayerController* PC = Cast<APlayerController>(OwnerPlayer->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(IMC_Museum, 1);
		}
	}

	// LV_MH_MyMuseum 레벨에서만 SaveGame 로드 및 유물 스폰
	if (UGameplayStatics::GetCurrentLevelName(GetWorld()) == TEXT("LV_MH_MyMuseum"))
	{
		if (UGI_Base* GI = Cast<UGI_Base>(UGameplayStatics::GetGameInstance(GetWorld())))
		{
			TArray<FRelicSaveData> SaveArray;
			GI->LoadRelicData(SaveArray);
			for (const FRelicSaveData& Data : SaveArray)
			{
				if (!Data.bIsPlaced) continue;
				
				const FCRelicData* Local_RelicData = GI->GetRelicDataByIndex(Data.RelicIndex);
				if (!Local_RelicData) continue;
				const FCRelicDetailData* Local_RelicDetailData = GI->GetRelicDetailDataByName(Local_RelicData->RelicName.ToString());
				if (!Local_RelicDetailData) continue;
				FActorSpawnParameters SpawnParams;
				SpawnParams.Owner = OwnerPlayer;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				ACRelicBase* RelicActor = GetWorld()->SpawnActor<ACRelicBase>(Local_RelicDetailData->RelicActorClass, Data.PlacedTransform, SpawnParams);
				if (RelicActor)
				{
					RelicActor->InitializeAsset(*Local_RelicData, *Local_RelicDetailData);
				}
			}
		}
	}
}

void UCMuseumComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                      FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsPreviewMode)
	{
		PreviewMode();
	}
}

void UCMuseumComponent::SetupPlayerInputComponent(UEnhancedInputComponent* EnhancedInput)
{
	EnhancedInput->BindAction(IA_Menu, ETriggerEvent::Started, this, &UCMuseumComponent::OnMenuButtonClicked);
}

void UCMuseumComponent::OnMenuButtonClicked()
{
	if (!OwnerPlayer) return;
	if (UGameplayStatics::GetCurrentLevelName(OwnerPlayer->GetWorld()) != FName("LV_MH_MyMuseum")) return;
	SwitchState();
}

void UCMuseumComponent::PreviewMode()
{
	FHitResult outHit;
	FVector start = OwnerPlayer->LAimMotionController->GetComponentLocation();
	FVector end = start + OwnerPlayer->LAimMotionController->GetForwardVector() * 600.f;
	FCollisionQueryParams params;
	params.AddIgnoredActor(OwnerPlayer);
	bool bHit = GetWorld()->LineTraceSingleByChannel(outHit, start, end, ECC_Visibility, params);;
	if (bHit)
	{
		BuildTransform.SetLocation(outHit.Location);
		BuildTransform.SetRotation(FRotator::ZeroRotator.Quaternion());
		// BuildTransform.SetRotation(BuildRotation.Quaternion());
		BuildTransform.SetScale3D(FVector(1));
		Relic->SetActorTransform(BuildTransform);
		Relic->SetRelicMaterial(RelicAcceptMaterial);
		bCanPlace = true;
	}
	else
	{
		BuildTransform.SetLocation(end);
		BuildTransform.SetRotation(FRotator::ZeroRotator.Quaternion());
		// BuildTransform.SetRotation(BuildRotation.Quaternion());
		BuildTransform.SetScale3D(FVector(1));
		Relic->SetActorTransform(BuildTransform);
		Relic->SetRelicMaterial(RelicRejectedMaterial);
		bCanPlace = false;
	}
}

void UCMuseumComponent::SwitchState()
{
	MuseumState = (EMuseumState)((MuseumState + 1) % EMuseumState::Max);

	switch (MuseumState)
	{
	case EMuseumState::Display:
		OwnerPlayer->RelicCollectionWidget->SetHiddenInGame(true);
		OwnerPlayer->LWidgetInteractionComponent->SetActive(false);
		OwnerPlayer->LWidgetInteractionComponent->bEnableHitTesting = false;
		OwnerPlayer->LWidgetInteractionComponent->bShowDebug = false;
		OwnerPlayer->RWidgetInteractionComponent->SetActive(false);
		OwnerPlayer->RWidgetInteractionComponent->bEnableHitTesting = false;
		OwnerPlayer->RWidgetInteractionComponent->bShowDebug = false;
		break;
	case EMuseumState::Decorate:
		OwnerPlayer->RelicCollectionWidget->SetHiddenInGame(false);
		OwnerPlayer->LWidgetInteractionComponent->SetActive(true);
		OwnerPlayer->LWidgetInteractionComponent->bEnableHitTesting = true;
		OwnerPlayer->LWidgetInteractionComponent->bShowDebug = true;
		OwnerPlayer->RWidgetInteractionComponent->SetActive(true);
		OwnerPlayer->RWidgetInteractionComponent->bEnableHitTesting = true;
		OwnerPlayer->RWidgetInteractionComponent->bShowDebug = true;
		break;
	}
}

void UCMuseumComponent::PlayPreviewMode(const FCRelicData& InRelicData, const FCRelicDetailData& InRelicDetailData)
{
	if (!OwnerPlayer) return;
	if (MuseumState != EMuseumState::Decorate) return;

	if (Relic->StaticClass() != InRelicDetailData.RelicActorClass)
	{
		if (Relic)
		{
			Relic->Destroy();
			Relic = nullptr;			
		}
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		Relic = GetWorld()->SpawnActor<ACRelicBase>(InRelicDetailData.RelicActorClass, SpawnParams);
		if (Relic)
		{
			Relic->SetActorEnableCollision(false);

			// 다이나믹 머터리얼 생성 및 적용
			if (Relic->GetRelicMesh() && Relic->GetRelicMesh()->GetMaterial(0))
			{
				RelicDynamicMaterial = Relic->GetRelicMesh()->CreateAndSetMaterialInstanceDynamic(0);
			}
		}
	}

	RelicData = InRelicData;
	RelicDetailData = InRelicDetailData;
	bIsPreviewMode = true;
}

void UCMuseumComponent::PlaceRelic()
{
	if (!bIsPreviewMode) return;
	if (!bCanPlace) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = OwnerPlayer;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	auto placeActor = GetWorld()->SpawnActor<ACRelicBase>(RelicDetailData.RelicActorClass, BuildTransform, SpawnParams);
	if (placeActor)
	{
		placeActor->InitializeAsset(RelicData, RelicDetailData);

		// SaveGame 저장
		if (UGI_Base* GI = Cast<UGI_Base>(UGameplayStatics::GetGameInstance(GetWorld())))
		{
			FRelicSaveData NewSaveData;
			NewSaveData.RelicIndex = RelicData.Index;
			NewSaveData.bIsPlaced = true;
			NewSaveData.PlacedTransform = BuildTransform;
			GI->SaveRelicData(NewSaveData);
		}
	}
	
	PreviewEnd();
	SwitchState();
}

void UCMuseumComponent::PreviewEnd()
{
	bIsPreviewMode = false;
	bCanPlace = false;
	Relic = nullptr;
	RelicDynamicMaterial = nullptr;
	RelicData = FCRelicData();
	RelicDetailData = FCRelicDetailData();
}