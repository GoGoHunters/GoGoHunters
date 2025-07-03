#pragma once

#include "CoreMinimal.h"

// GameFramework
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

// Components
#include "Components/CapsuleComponent.h"

// Niagara
#include "Particles/ParticleSystem.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"

class GOGOHUNTERS_API CHelpers
{
public:
	template<typename T>
	static void GetAsset(T** OutObject, FString InPath)
	{
		ConstructorHelpers::FObjectFinder<T> asset(*InPath);
		*OutObject = asset.Object;
	}

	template<typename T>
	static void GetStaticAsset(T** OutObject, const FString& InPath)
	{
		ConstructorHelpers::FObjectFinder<UStaticMesh> asset(*InPath);
		(*OutObject)->SetStaticMesh(asset.Object);
	}

	template<typename T>
	static void GetAssetDynamic(T** OutObject, FString InPath)
	{
		*OutObject = Cast<T>(StaticLoadObject(T::StaticClass(), nullptr, *InPath));
	}

	template<typename T>
	static void GetClass(TSubclassOf<T>* OutClass, FString InPath)
	{
		ConstructorHelpers::FClassFinder<T> asset(*InPath);
		*OutClass = asset.Class;
	}

	template<typename T>
	static T* GetComponent(AActor* InActor)
	{
		return Cast<T>(InActor->GetComponentByClass(T::StaticClass()));
	}

	template<typename T>
	static T* GetComponent(AActor* InActor, const FString& InName)
	{
		TArray<T*> components;
		InActor->GetComponents<T>(components);

		for (T* component : components)
		{
			if (component->GetName() == InName)
				return component;
		}

		return nullptr;
	}

	template<typename T>
	static void CreateComponent(AActor* InActor, T** OutComponent, FName InName, USceneComponent* InParent = nullptr, FName InSocketName = NAME_None)
	{
		*OutComponent = InActor->CreateDefaultSubobject<T>(InName);

		if (!!InParent)
		{
			(*OutComponent)->SetupAttachment(InParent, InSocketName);

			return;
		}

		InActor->SetRootComponent(*OutComponent);
	}

	template<typename T>
	static void CreateActorComponent(AActor* InActor, T** OutComponent, FName InName)
	{
		*OutComponent = InActor->CreateDefaultSubobject<T>(InName);
	}

	template<typename T>
	static T* FindActor(UWorld* InWorld)
	{
		for (AActor* actor : InWorld->GetCurrentLevel()->Actors)
		{
			if (!!actor && actor->IsA<T>())
				return Cast<T>(actor);
		}

		return nullptr;
	}

	template<typename T>
	static void FindActors(UWorld* InWorld, TArray<T*>& OutActors)
	{
		for (AActor* actor : InWorld->GetCurrentLevel()->Actors)
		{
			if (!!actor && actor->IsA<T>())
				OutActors.Add(Cast<T>(actor));
		}
	}

	static void AttachTo(AActor* InActor, USceneComponent* InParent, FName InSocketName)
	{
		InActor->AttachToComponent(InParent, FAttachmentTransformRules(EAttachmentRule::KeepRelative, true), InSocketName);
	}

	static void PlayEffect(UWorld* InWorld, UFXSystemAsset* InAsset, const FTransform& InTransform, USkeletalMeshComponent* InMesh = nullptr, FName InSocketName = NAME_None)
	{
		UParticleSystem* particle = Cast<UParticleSystem>(InAsset);
		UNiagaraSystem* niagara = Cast<UNiagaraSystem>(InAsset);

		FVector location = InTransform.GetLocation();
		FRotator rotation = FRotator(InTransform.GetRotation());
		FVector scale = InTransform.GetScale3D();


		if (!!InMesh)
		{
			if (!!particle)
			{
				UGameplayStatics::SpawnEmitterAttached(particle, InMesh, InSocketName, location, rotation, scale);

				return;
			}

			if (!!niagara)
			{
				UNiagaraFunctionLibrary::SpawnSystemAttached(niagara, InMesh, InSocketName, location, rotation, scale, EAttachLocation::KeepRelativeOffset, true, ENCPoolMethod::None);

				return;
			}
		}

		if (!!particle)
		{
			UGameplayStatics::SpawnEmitterAtLocation(InWorld, particle, InTransform);

			return;
		}

		if (!!niagara)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(InWorld, niagara, location, rotation, scale);

			return;
		}
	}

};
