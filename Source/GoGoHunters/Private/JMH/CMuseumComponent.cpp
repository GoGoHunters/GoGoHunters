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
				ACMuseumPlaceArea* FoundArea = nullptr;
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
						if (Data.PlaceArea) Data.PlaceArea->PlaceRelicAt(Data.PlacedTransform.GetLocation());
					}
				}				
			}
		}
	}

	if (OwnerPlayer)
	{
		GrabComponent = OwnerPlayer->GetComponentByClass<UMH_GrabComp>();
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
	// EnhancedInput->BindAction(IA_Menu, ETriggerEvent::Started, this, &UCMuseumComponent::OnMenuButtonClicked);
	EnhancedInput->BindAction(IA_Menu, ETriggerEvent::Started, this, &UCMuseumComponent::ToggleMenu);
}

void UCMuseumComponent::OnMenuButtonClicked()
{
	if (!OwnerPlayer) return;
	if (!UGameplayStatics::GetCurrentLevelName(GetWorld()).Contains(MuseumLevelName)) return;
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
		PlaceArea = Cast<ACMuseumPlaceArea>(outHit.GetActor());
		
		// PlaceArea에 닿았으면, 가장 가까운 GridCell의 Center로 스냅
		if (PlaceArea)
		{
			const TArray<FGridCell>& GridCells = PlaceArea->GetGridCells();
			float MinDist = TNumericLimits<float>::Max();
			FVector ClosestCenter = outHit.Location;
			FVector ClosestScale = FVector(1.f);
			for (const FGridCell& Cell : GridCells)
			{
				float Dist = FVector::Dist2D(Cell.Center, outHit.Location);
				if (Dist < MinDist)
				{
					MinDist = Dist;
					ClosestCenter = Cell.Center;
					ClosestScale = Cell.Scale;
				}
			}
			BuildTransform.SetLocation(ClosestCenter);
			BuildTransform.SetScale3D(ClosestScale);
			BuildTransform.SetRotation(FRotator::ZeroRotator.Quaternion());
			
			Relic->SetActorTransform(BuildTransform);

			if (!PlaceArea->CanPlaceRelicAt(outHit.Location))
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
		GrabComponent->RelicUnGrab();
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
		placeActor->SetActorScale3D(BuildTransform.GetScale3D());
		PlaceArea->PlaceRelicAt(BuildTransform.GetLocation());
		placeActor->InitializeAsset(RelicData, RelicDetailData);
		placeActor->SetRelicMaterial();
		placeActor->Tags.Add("Grabable");
		
		// SaveGame 저장
		if (UGI_Base* GI = Cast<UGI_Base>(UGameplayStatics::GetGameInstance(GetWorld())))
		{
			RelicData.PlacedTransform = BuildTransform;
			RelicData.IsPlace = true;
			RelicData.PlaceArea = PlaceArea;
			
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

void UCMuseumComponent::RegisterRelic(const int32& InRelicTag)
{
	if (InRelicTag == -1) return;
	if (UGI_Base* GI = Cast<UGI_Base>(UGameplayStatics::GetGameInstance(GetWorld())))
	{
		const FCRelicDetailData* l_RelicDetailData = GI->GetRelicDetailDataByTag(InRelicTag);

		if (!l_RelicDetailData) return;
		
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
	}
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
	// 1. 범위 내 ACMuseumPlaceArea 찾기
	TArray<ACMuseumPlaceArea*> NearbyAreas;
	for (TActorIterator<ACMuseumPlaceArea> It(GetWorld()); It; ++It)
	{
		ACMuseumPlaceArea* Area = *It;
		if (FVector::Dist(Area->GetActorLocation(), HandComponentLocation) <= RePlaceAreaSearchRange)
		{
			NearbyAreas.Add(Area);
		}
	}

	// 가까운 순서대로 정렬
	Algo::SortBy(NearbyAreas, [HandComponentLocation](const ACMuseumPlaceArea* Area)
	{
		return FVector::Dist(Area->GetActorLocation(), HandComponentLocation);
	});

	// 2. 빈 칸 찾기 및 등록
	bool bPlaced = false;
	for (ACMuseumPlaceArea* Area : NearbyAreas)
	{
		FVector EmptySlotLocation = Area->FindEmptySlot(HandComponentLocation);
		if (EmptySlotLocation != FVector::ZeroVector)
		{
			// 3-1. 원래 칸에서 Relic 해제
			if (GrabRelic->GetPlaceAreaActor())
			{
				GrabRelic->GetPlaceAreaActor()->UnregisterRelic(GrabRelic);
			}
			// 3-2. 새 칸에 등록
			Area->PlaceRelicAt(EmptySlotLocation);
			Area->SetPlaceRelicAtLocation(GrabRelic, EmptySlotLocation);
			bPlaced = true;
			break;
		}
	}

	// 4. 빈 칸이 없으면 원래 위치로 이동
	if (!bPlaced)
	{
		GrabRelic->ReturnToOriginalLocation();
	}

	// 5. 피직스 끄기
	if (UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(GrabRelic->GetRootComponent()))
		PrimComp->SetSimulatePhysics(false);	
}

bool UCMuseumComponent::FindNearbyPlaceArea(const FVector& Location, float SearchRadius, ACMuseumPlaceArea*& OutArea, FVector& OutCellScale) const
{
    OutArea = nullptr;
    OutCellScale = FVector(1.f);

    // 탐색 범위 시각화 (빨간색 구체)
    // DrawDebugSphere(GetWorld(), Location, SearchRadius, 12, FColor::Red, false, 5.0f, 0, 2.0f);

    TArray<ACMuseumPlaceArea*> CandidateAreas;

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
            if (ACMuseumPlaceArea* Area = Cast<ACMuseumPlaceArea>(HitActor))
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

    Algo::SortBy(CandidateAreas, [Location](const ACMuseumPlaceArea* Area)
    {
        return FVector::Dist(Area->GetActorLocation(), Location);
    });

    // 가장 가까운 영역 선택 후, 해당 위치에 가장 가까운 GridCell의 스케일 가져오기
    ACMuseumPlaceArea* Nearest = CandidateAreas[0];
    if (!Nearest) return false;

    // 가장 가까운 영역 시각화 (초록색 박스)
    // DrawDebugBox(GetWorld(), Nearest->GetActorLocation(), FVector(60.f), FColor::Green, false, 5.0f, 0, 4.0f);

    const TArray<FGridCell>& Cells = Nearest->GetGridCells();
    float MinDist = TNumericLimits<float>::Max();
    FVector ClosestScale = FVector(1.f);
    for (const FGridCell& Cell : Cells)
    {
        const float Dist = FVector::Dist(Cell.Center, Location);
        if (Dist < MinDist)
        {
            MinDist = Dist;
            ClosestScale = Cell.Scale;
        }
    }

    // 가장 가까운 셀 중심 시각화 (보라색 구체)
    // DrawDebugSphere(GetWorld(), ClosestCellCenter, 15.f, 8, FColor::Purple, false, 5.0f, 0, 5.0f);

    OutArea = Nearest;
    OutCellScale = ClosestScale;
    return true;
}
