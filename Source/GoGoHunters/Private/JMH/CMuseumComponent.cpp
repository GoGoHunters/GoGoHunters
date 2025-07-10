#include "JMH/CMuseumComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "MotionControllerComponent.h"
#include "Components/WidgetInteractionComponent.h"
#include "JMH/MH_VRPlayer.h"
#include "LHJ/CMuseumActorBase.h"
#include "LHJ/CRelicBase.h"
#include "Utilities/CHelpers.h"

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

void UCMuseumComponent::OnSelectItemButtonClicked(const FInputActionInstance& IA_Instance)
{
	if (MuseumState != EMuseumState::Decorate) return;
	if (!OwnerPlayer) return;

	APlayerController* PC = Cast<APlayerController>(OwnerPlayer->GetController());
	if (!PC) return;

	UMotionControllerComponent* MotionController = nullptr;
	if (IA_Instance.GetSourceAction() == OwnerPlayer->IA_MHInteract) MotionController = OwnerPlayer->RHandController;
	else if (IA_Instance.GetSourceAction() == OwnerPlayer->IA_MHInteract_L) MotionController = OwnerPlayer->
		LHandController;

	if (!MotionController) return;

	// 선택된 오브젝트가 없으면: 라인트레이스 등으로 선택
	if (!SelectedActor)
	{
		FVector Start = MotionController->GetComponentLocation();
		FVector End = Start + (MotionController->GetForwardVector() * 600.f);

		FHitResult Hit;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(OwnerPlayer);

		if (OwnerPlayer->GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
		{
			if (Hit.GetActor() && Hit.GetActor()->IsA(ACMuseumActorBase::StaticClass()))
			{
				SelectedActor = Hit.GetActor();
				UE_LOG(LogTemp, Warning, TEXT("SelectedActor: %s"), *SelectedActor->GetName());
				// 선택 효과(예: Outline 등) 추가 가능
			}
		}
	}
	// 선택된 오브젝트가 있으면: 현재 위치에 배치
	else
	{
		FVector Start = MotionController->GetComponentLocation();
		FVector End = Start + (MotionController->GetForwardVector() * 600.f);

		FHitResult Hit;
		if (OwnerPlayer->GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility))
		{
			FVector TargetLocation = Hit.Location;
			SelectedActor->SetActorLocation(TargetLocation);
			UE_LOG(LogTemp, Warning, TEXT("SelectedActor End: %s"), *SelectedActor->GetName());
			// 필요시 회전/스케일도 조정 가능
			SelectedActor = nullptr; // 선택 해제
		}
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
