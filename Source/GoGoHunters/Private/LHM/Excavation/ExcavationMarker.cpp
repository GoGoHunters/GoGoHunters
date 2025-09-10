// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/Excavation/ExcavationMarker.h"
#include "../../../../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraFunctionLibrary.h"

// Sets default values
AExcavationMarker::AExcavationMarker()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AExcavationMarker::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AExcavationMarker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsAppearing)
	{
		AppearingElapsedSeconds += DeltaTime;
		const float clampedT = FMath::Clamp(AppearingElapsedSeconds / FMath::Max(0.001f, AppearDurationSeconds), 0.0f, 1.0f);

		// easeOutBounce implementation
		auto EaseOutBounce = [](float t) -> float
		{
			const float n1 = 7.5625f;
			const float d1 = 2.75f;
			if (t < 1.0f / d1)
			{
				return n1 * t * t;
			}
			else if (t < 2.0f / d1)
			{
				t -= 1.5f / d1;
				return n1 * t * t + 0.75f;
			}
			else if (t < 2.5f / d1)
			{
				t -= 2.25f / d1;
				return n1 * t * t + 0.9375f;
			}
			else
			{
				t -= 2.625f / d1;
				return n1 * t * t + 0.984375f;
			}
		};

		const float eased = EaseOutBounce(clampedT);

		// Scale bounce (Z only)
		FVector newScale = TargetWorldScale;
		newScale.Z = FMath::Lerp(InitialWorldScale.Z, TargetWorldScale.Z, eased);
		SetActorScale3D(newScale);

		// Drop-from-above position bounce
		if (bEnableDropEffect)
		{
			const FVector newLocation = FMath::Lerp(DropStartLocation, TargetWorldLocation, eased);
			SetActorLocation(newLocation);
		}

		if (clampedT >= 1.0f)
		{
			bIsAppearing = false;
			SetActorScale3D(TargetWorldScale);
			if (bEnableDropEffect)
			{
				SetActorLocation(TargetWorldLocation);
			}
		}
	}
}

void AExcavationMarker::ActivateMarker()
{
	SetActorHiddenInGame(false);
	// Begin appear animation
	TargetWorldScale = GetActorScale3D();
	InitialWorldScale = FVector(TargetWorldScale.X, TargetWorldScale.Y, 0.0f);
	SetActorScale3D(InitialWorldScale);
	AppearingElapsedSeconds = 0.0f;
	bIsAppearing = true;

	// Setup drop-from-above
	TargetWorldLocation = GetActorLocation();
	if (bEnableDropEffect)
	{
		DropStartLocation = TargetWorldLocation + FVector(0.0f, 0.0f, DropHeight);
		SetActorLocation(DropStartLocation);
	}
	
	// 이펙트, 깃발 표시 등
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), PulseEffect, GetActorLocation());
	
	// 머티리얼에 하이라이트 효과 추가

}

