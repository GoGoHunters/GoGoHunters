#include "JMH/CMuseumComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "MotionControllerComponent.h"
#include "JMH/MH_VRPlayer.h"
#include "LHJ/CMuseumActorBase.h"
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

	if (MuseumState == EMuseumState::Decorate)
	{
		FVector Start = OwnerPlayer->RHandController->GetComponentLocation();
		FVector End = Start + (OwnerPlayer->RHandController->GetForwardVector() * 600.f);
		DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 0.f, 0, 1);
		Start = OwnerPlayer->LHandController->GetComponentLocation();
		End = Start + (OwnerPlayer->LHandController->GetForwardVector() * 600.f);
		DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 0.f, 0, 1);
	}
}

void UCMuseumComponent::SetupPlayerInputComponent(UEnhancedInputComponent* EnhancedInput)
{
	EnhancedInput->BindAction(IA_Menu, ETriggerEvent::Started, this, &UCMuseumComponent::OnMenuButtonClicked);
	EnhancedInput->BindAction(IA_SelectItem_R, ETriggerEvent::Started, this,
	                          &UCMuseumComponent::OnSelectItemButtonClicked);
	EnhancedInput->BindAction(IA_SelectItem_L, ETriggerEvent::Started, this,
	                          &UCMuseumComponent::OnSelectItemButtonClicked);
}

void UCMuseumComponent::OnMenuButtonClicked()
{
	if (!OwnerPlayer) return;
	if (UGameplayStatics::GetCurrentLevelName(OwnerPlayer->GetWorld()) != FName("LV_MH_MyMuseum")) return;
	SwitchState();
}

void UCMuseumComponent::SwitchState()
{
	MuseumState = (EMuseumState)((MuseumState + 1) % EMuseumState::Max);

	switch (MuseumState)
	{
	case EMuseumState::Display:
		break;
	case EMuseumState::Decorate:
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
	if (IA_Instance.GetSourceAction() == IA_SelectItem_R) MotionController = OwnerPlayer->RHandController;
	else if (IA_Instance.GetSourceAction() == IA_SelectItem_L) MotionController = OwnerPlayer->LHandController;

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
