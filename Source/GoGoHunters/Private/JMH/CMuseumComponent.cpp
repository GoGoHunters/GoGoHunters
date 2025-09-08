#include "JMH/CMuseumComponent.h"

#include "EngineUtils.h"
#include "Algo/Sort.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "MotionControllerComponent.h"
#include "Components/WidgetInteractionComponent.h"
#include "JMH/MH_VRPlayer.h"
#include "LHJ/CMuseumActorBase.h"
#include "LHJ/CRelicBase.h"
#include "Utilities/CHelpers.h"
#include "base/GI_Base.h"
#include "JMH/MH_GrabComp.h"
#include "LHJ/CMuseumPlaceArea.h"
#include "LHJ/CRelicCollectionWidgetActor.h"
#include "DrawDebugHelpers.h"
#include "Engine/OverlapResult.h"
#include "LHJ/CRelicPlaceActor.h"

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
	
	if (OwnerPlayer)
	{
		GrabComponent = OwnerPlayer->GetComponentByClass<UMH_GrabComp>();
	}

	LoadPlacedRelic();
	// OnMakeGridCompleted.BindUFunction(this, FName("LoadPlacedRelic"));
	// bBeginPlayEnded = true;
}

void UCMuseumComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                      FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsPreviewMode)
	{
		PreviewMode();
	}

	if (bIsGrabbing)
	{
		SetRelicScaleToGrabScale();
	}
}

void UCMuseumComponent::SetupPlayerInputComponent(UEnhancedInputComponent* EnhancedInput)
{
	// EnhancedInput->BindAction(IA_Menu, ETriggerEvent::Started, this, &UCMuseumComponent::OnMenuButtonClicked);
	EnhancedInput->BindAction(IA_Menu, ETriggerEvent::Started, this, &UCMuseumComponent::ToggleMenu);
}

void UCMuseumComponent::OnMenuButtonClicked()
{
	if (!OwnerPlayer) return;
	if (!UGameplayStatics::GetCurrentLevelName(GetWorld()).Contains(MuseumLevelName)) return;

	if (MuseumState == Decorate)
		SwitchState();
}

void UCMuseumComponent::PreviewMode()
{
	if (!OwnerPlayer) return;
	if (!OwnerPlayer->RWidgetInteractionComponent) return;

	OwnerPlayer->SetUseLineTraceEffect(true);
	
	FHitResult outHit;
	FVector start = OwnerPlayer->RAimMotionController->GetComponentLocation();
	FVector end = start + OwnerPlayer->RAimMotionController->GetForwardVector() * OwnerPlayer->GetWidgetInteractionDistance();
	FCollisionQueryParams params;
	params.AddIgnoredActor(OwnerPlayer);
	bool bHit = GetWorld()->LineTraceSingleByChannel(outHit, start, end, ECC_GameTraceChannel6, params);;
	if (bHit)
	{
		PlaceArea = Cast<ACRelicPlaceActor>(outHit.GetActor());
		
		// PlaceArea에 닿았으면, 가장 가까운 GridCell의 Center로 스냅
		if (PlaceArea)
		{
			BuildTransform.SetLocation(PlaceArea->GetActorLocation());
			BuildTransform.SetScale3D(PlaceArea->GetPlaceMeshScale());
			BuildTransform.SetRotation(FRotator::ZeroRotator.Quaternion());
			
			Relic->SetActorTransform(BuildTransform);

			if (!PlaceArea->CanPlaceRelic())
			{
				Relic->SetRelicMaterial(RelicRejectedMaterial);
				bCanPlace = false;
			}
			else
			{
				Relic->SetRelicMaterial(RelicAcceptMaterial);
				bCanPlace = true;				
			}
		}

		end = outHit.ImpactPoint;
		OwnerPlayer->UpdateDrawLineTraceEffect(start, end);
	}
	else
	{
		BuildTransform.SetLocation(end);
		BuildTransform.SetRotation(FRotator::ZeroRotator.Quaternion());
		BuildTransform.SetScale3D(FVector(1));
		Relic->SetActorTransform(BuildTransform);
		Relic->SetRelicMaterial(RelicRejectedMaterial);
		bCanPlace = false;

		PlaceArea = nullptr;
		
		OwnerPlayer->UpdateDrawLineTraceEffect(start, end);
	}
}

void UCMuseumComponent::SwitchState()
{
	if (!UGameplayStatics::GetCurrentLevelName(GetWorld()).Contains(MuseumLevelName)) return;
	
	MuseumState = (EMuseumState)((MuseumState + 1) % EMuseumState::Max);

	switch (MuseumState)
	{
	case EMuseumState::Display:
		OwnerPlayer->RWidgetInteractionComponent->SetActive(false);
		OwnerPlayer->RWidgetInteractionComponent->bEnableHitTesting = false;
		
		bIsPreviewMode = false;
		PreviewEnd();
		OwnerPlayer->SetUseLineTraceEffect(false);
		break;
		
	case EMuseumState::Decorate:
		OwnerPlayer->RWidgetInteractionComponent->SetActive(true);
		OwnerPlayer->RWidgetInteractionComponent->bEnableHitTesting = true;
		OwnerPlayer->DropForMuseumStateChange();
		break;
	}
}

void UCMuseumComponent::ToggleMenu()
{
	if (!OwnerPlayer) return;
	OwnerPlayer->ToggleMenu();
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
	if (OnUiAnimPlay.IsBound())
		OnUiAnimPlay.Execute(true);
}

void UCMuseumComponent::PlaceRelic()
{
	if (!bIsPreviewMode) return;
	if (!bCanPlace) return;

	bIsPreviewMode = false;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = OwnerPlayer;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	auto placeActor = GetWorld()->SpawnActor<ACRelicBase>(RelicDetailData.RelicActorClass, BuildTransform, SpawnParams);
	if (placeActor)
	{
		RelicData.PlacedTransform = BuildTransform;
		RelicData.IsPlace = true;
		RelicData.PlaceArea = PlaceArea;
		
		placeActor->SetActorScale3D(BuildTransform.GetScale3D());
		placeActor->InitializeAsset(RelicData, RelicDetailData);
		PlaceArea->RegisterRelic(placeActor);
		placeActor->SetRelicMaterial();
		placeActor->Tags.Add("Grabable");
		placeActor->SetRelicGrabScale();
		
		// SaveGame 저장
		if (UGI_Base* GI = Cast<UGI_Base>(UGameplayStatics::GetGameInstance(GetWorld())))
		{
			FRelicSaveData NewSaveData;
			NewSaveData.RelicData = RelicData;
			GI->SaveRelicData(NewSaveData);
		}
	}

	PreviewEnd();

	if (OnRelicPlace.IsBound())
		OnRelicPlace.Execute();
	if (OnUiAnimPlay.IsBound())
		OnUiAnimPlay.Execute(false);
}

FCRelicData UCMuseumComponent::RegisterRelic(const int32& InRelicTag)
{
	if (InRelicTag == -1) return FCRelicData();
	if (UGI_Base* GI = Cast<UGI_Base>(UGameplayStatics::GetGameInstance(GetWorld())))
	{
		const FCRelicDetailData* l_RelicDetailData = GI->GetRelicDetailDataByTag(InRelicTag);

		if (!l_RelicDetailData) return FCRelicData();
		
		FCRelicData NewRelicData;
		NewRelicData.RelicName = l_RelicDetailData->RelicName;
		NewRelicData.RelicTag = InRelicTag;
		NewRelicData.DropDate = FDateTime::Now();
		NewRelicData.PlacedTransform = FTransform();
		NewRelicData.IsPlace = false;
		NewRelicData.PlaceArea = nullptr;

		FRelicSaveData NewSaveData;
		NewSaveData.RelicData = NewRelicData;
		GI->SaveRelicData(NewSaveData);

		return NewRelicData;
	}
	return FCRelicData();
}

void UCMuseumComponent::RegisterRelicCollectorName(FCRelicData& InRelicData, FName InCollectorName)
{
	if (InCollectorName == NAME_None) return;
	
	if (UGI_Base* GI = Cast<UGI_Base>(UGameplayStatics::GetGameInstance(GetWorld())))
	{
		InRelicData.CollectorName = InCollectorName;
		FRelicSaveData NewSaveData;
		NewSaveData.RelicData = InRelicData;
		GI->SaveRelicData(NewSaveData);
	}
}

void UCMuseumComponent::RecoverRelic(FCRelicData& InRelicData)
{
	if (UGI_Base* GI = Cast<UGI_Base>(UGameplayStatics::GetGameInstance(GetWorld())))
	{
		InRelicData.PlacedTransform = FTransform();
		InRelicData.IsPlace = false;
		InRelicData.PlaceArea = nullptr;

		FRelicSaveData NewSaveData;
		NewSaveData.RelicData = InRelicData;
		GI->SaveRelicData(NewSaveData);
	}

	if (OnRelicPlace.IsBound())
		OnRelicPlace.Execute();
}

void UCMuseumComponent::GrabRelic(ACRelicBase* GrabRelic)
{
	if (!GrabRelic) return;
	GrabbedRelic = GrabRelic;
	bIsGrabbing = true;
}

void UCMuseumComponent::PreviewEnd()
{
	if (Relic) Relic->Destroy();
	bCanPlace = false;
	Relic = nullptr;
	RelicDynamicMaterial = nullptr;
	RelicData = FCRelicData();
	RelicDetailData = FCRelicDetailData();
	PlaceArea = nullptr;
}

void UCMuseumComponent::GrabRelicEnd(ACRelicBase* GrabRelic, const FVector& HandComponentLocation)
{
	GrabbedRelic = nullptr;

	// 관람 모드일때는 유물 이동이 안되도록 변경
	if (MuseumState==Display)
	{
		GrabRelic->ReturnToOriginalLocation();
		if (UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(GrabRelic->GetRootComponent()))
			PrimComp->SetSimulatePhysics(false);
		return;
	}
	
	// 1. 범위 내 ACMuseumPlaceArea 찾기
	TArray<ACRelicPlaceActor*> NearbyAreas;
	for (TActorIterator<ACRelicPlaceActor> It(GetWorld()); It; ++It)
	{
		ACRelicPlaceActor* Area = *It;
		if (FVector::Dist(Area->GetActorLocation(), HandComponentLocation) <= RePlaceAreaSearchRange)
		{
			NearbyAreas.Add(Area);
		}
	}

	// 가까운 순서대로 정렬
	Algo::SortBy(NearbyAreas, [HandComponentLocation](const ACRelicPlaceActor* Area)
	{
		return FVector::Dist(Area->GetActorLocation(), HandComponentLocation);
	});

	// 2. 빈 칸 찾기 및 등록
	bool bPlaced = false;
	int32 PlaceAreaIndex = 0;
	for (int32 i = 0; i < NearbyAreas.Num(); ++i)
	{
		bPlaced = NearbyAreas[i]->CanPlaceRelic();
		if (!bPlaced) continue;
		
		PlaceAreaIndex = i;
		break;
	}

	if (bPlaced)
	{
		ACRelicPlaceActor* FoundArea = nullptr;
		FVector FoundScale = FVector(1.f);
		// 3-1. 원래 칸에서 Relic 해제
		if (FindNearbyPlaceArea(GrabRelic->GetRelicPlaceLocation(), 10.f, FoundArea, FoundScale))
		{
			FoundArea->UnRegisterRelic();
		}
		// 3-2. 새 칸에 등록
		NearbyAreas[PlaceAreaIndex]->RegisterRelic(GrabRelic);
		NearbyAreas[PlaceAreaIndex]->SetPlaceRelicAtLocation(GrabRelic);
		
		// 배치 위치 업데이트
		FCRelicData PlacedRelicData = GrabRelic->UpdateRelicLocation(NearbyAreas[PlaceAreaIndex]->GetActorLocation());
		if (UGI_Base* GI = Cast<UGI_Base>(UGameplayStatics::GetGameInstance(GetWorld())))
		{
			FRelicSaveData NewSaveData;
			NewSaveData.RelicData = PlacedRelicData;
			GI->SaveRelicData(NewSaveData);
		}
	}
	// 4. 빈 칸이 없으면 원래 위치로 이동
	else if (!bPlaced)
	{
		GrabRelic->ReturnToOriginalLocation();
	}

	// 5. 피직스 끄기
	if (UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(GrabRelic->GetRootComponent()))
		PrimComp->SetSimulatePhysics(false);	
}

bool UCMuseumComponent::FindNearbyPlaceArea(const FVector& Location, float SearchRadius, ACRelicPlaceActor*& OutArea, FVector& OutCellScale) const
{
    OutArea = nullptr;
    OutCellScale = FVector(1.f);

    // 탐색 범위 시각화 (빨간색 구체)
    // DrawDebugSphere(GetWorld(), Location, SearchRadius, 12, FColor::Red, false, 5.0f, 0, 2.0f);

    TArray<ACRelicPlaceActor*> CandidateAreas;

    // 거리 대신 Overlap으로 탐지
    TArray<FOverlapResult> Overlaps;
    FCollisionObjectQueryParams ObjectQueryParams = FCollisionObjectQueryParams::AllObjects;
    FCollisionQueryParams QueryParams;
    QueryParams.bTraceComplex = false;
    QueryParams.AddIgnoredActor(GetOwner());

    const bool bAnyOverlap = GetWorld()->OverlapMultiByObjectType(
        Overlaps,
        Location,
        FQuat::Identity,
        ObjectQueryParams,
        FCollisionShape::MakeSphere(SearchRadius),
        QueryParams
    );

    if (bAnyOverlap)
    {
        for (const FOverlapResult& Result : Overlaps)
        {
            AActor* HitActor = Result.GetActor();
            if (!HitActor) continue;
            if (ACRelicPlaceActor* Area = Cast<ACRelicPlaceActor>(HitActor))
            {
                CandidateAreas.AddUnique(Area);
                // 후보 영역 시각화 (파란색 박스)
                // DrawDebugBox(GetWorld(), Area->GetActorLocation(), FVector(50.f), FColor::Blue, false, 5.0f, 0, 3.0f);
            }
        }
    }

    if (CandidateAreas.Num() == 0)
    {
        // 탐색 결과 없음 시각화 (노란색 구체)
        // DrawDebugSphere(GetWorld(), Location, 20.f, 8, FColor::Yellow, false, 5.0f, 0, 1.0f);
        return false;
    }

    Algo::SortBy(CandidateAreas, [Location](const ACRelicPlaceActor* Area)
    {
        return FVector::Dist(Area->GetActorLocation(), Location);
    });

    // 가장 가까운 영역 선택 후, 해당 위치에 가장 가까운 GridCell의 스케일 가져오기
    ACRelicPlaceActor* Nearest = CandidateAreas[0];

    // 가장 가까운 영역 시각화 (초록색 박스)
    // DrawDebugBox(GetWorld(), Nearest->GetActorLocation(), FVector(60.f), FColor::Green, false, 5.0f, 0, 4.0f);

    OutArea = Nearest;
    OutCellScale = OutArea->GetPlaceMeshScale();
    return true;
}

void UCMuseumComponent::SetRelicScaleToGrabScale()
{
	if (!GrabbedRelic)
	{
		bIsGrabbing = false;
		return;
	}

	// 현재 스케일과 목표 스케일 사이를 Lerp로 부드럽게 보간
	FVector CurrentScale = GrabbedRelic->GetActorScale3D();
	FVector TargetScale = GrabRelicScale;
		
	FVector NewScale = FMath::Lerp(CurrentScale, TargetScale, LerpScale);
	
	// NewScale이 GrabRelicScale과 0.01 이상 차이가 나지 않으면 GrabRelicScale로 직접 설정
	if (FVector::Dist(NewScale, TargetScale) <= 0.01)
	{
		GrabbedRelic->SetActorScale3D(TargetScale);
		bIsGrabbing = false;
		GrabbedRelic = nullptr;
	}
	else
	{
		GrabbedRelic->SetActorScale3D(NewScale);
	}
}

void UCMuseumComponent::LoadPlacedRelic()
{	
	// LV_MH_MyMuseum 레벨에서만 SaveGame 로드 및 유물 스폰
	if (UGameplayStatics::GetCurrentLevelName(GetWorld()).Contains(MuseumLevelName))
	{
		if (UGI_Base* GI = Cast<UGI_Base>(UGameplayStatics::GetGameInstance(GetWorld())))
		{
			TArray<FCRelicData> RelicArray = GI->GetAllRelicData();
			for (FCRelicData& Data : RelicArray)
			{
				if (!Data.IsPlace) continue;
				if (Data.RelicTag == -1) continue;
				
				const FCRelicDetailData* Local_RelicDetailData = GI->GetRelicDetailDataByTag(Data.RelicTag);

				if (!Local_RelicDetailData) continue;

				// 저장된 PlaceArea 포인터에 의존하지 않고, PlacedTransform 위치 주변(반경 10)에서 PlaceArea를 탐색하여 스케일을 반영
				ACRelicPlaceActor* FoundArea = nullptr;
				FVector FoundScale = FVector(1.f);
				if (FindNearbyPlaceArea(Data.PlacedTransform.GetLocation(), 10.f, FoundArea, FoundScale))
				{
					Data.PlacedTransform.SetScale3D(FoundScale);
					Data.PlaceArea = FoundArea; // 유효하면 갱신
				}

				if (FoundArea)
				{
					FActorSpawnParameters SpawnParams;
					SpawnParams.Owner = OwnerPlayer;
					SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;                
				
					ACRelicBase* RelicActor = GetWorld()->SpawnActor<ACRelicBase>(Local_RelicDetailData->RelicActorClass, Data.PlacedTransform, SpawnParams);
					if (RelicActor)
					{
						RelicActor->SetActorScale3D(Data.PlacedTransform.GetScale3D());
						RelicActor->InitializeAsset(Data, *Local_RelicDetailData);
						RelicActor->Tags.Add("Grabable");
						FoundArea->RegisterRelic(RelicActor);
						RelicActor->SetRelicGrabScale();

						// RelicActor->SimulatePhysics(true);
						// FTimerHandle hnd;
						// GetWorld()->GetTimerManager().SetTimer(hnd, [RelicActor]()
						// {
						// 	RelicActor->SimulatePhysics(false);
						// }, 0.4f, false);						
					}
				}				
			}
		}
	}
}
