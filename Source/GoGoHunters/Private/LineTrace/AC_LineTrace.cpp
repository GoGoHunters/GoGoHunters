// Fill out your copyright notice in the Description page of Project Settings.


#include "LineTrace/AC_LineTrace.h"
#include "Engine/Engine.h"
#include "DrawDebugHelpers.h"
#include "Components/ArrowComponent.h"
#include "Engine/World.h"

UAC_LineTrace::UAC_LineTrace()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.
    PrimaryComponentTick.bCanEverTick = false;
}

void UAC_LineTrace::BeginPlay()
{
    Super::BeginPlay();
}

void UAC_LineTrace::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    PerformLineTrace();
}



bool UAC_LineTrace::AttachToSocket(USceneComponent* InParentComponent, FName InSocketName = NAME_None)
{
    if (!ParentComponent)
        return false;

    ParentComponent = InParentComponent;
    bool bAttached = AttachToComponent(ParentComponent, FAttachmentTransformRules::KeepRelativeTransform);

    if (InSocketName != NAME_None)
    {
        if (USkeletalMeshComponent* SkeletalMeshComp = Cast<USkeletalMeshComponent>(ParentComponent))
        {
            if (!SkeletalMeshComp->DoesSocketExist(InSocketName))
            {
                UE_LOG(LogTemp, Warning, TEXT("Socket '%s' does not exist on skeletal mesh component"), *InSocketName.ToString());
                return bAttached;
            }
        }
        else if (UStaticMeshComponent* StaticMeshComp = Cast<UStaticMeshComponent>(ParentComponent))
        {
            if (!StaticMeshComp->DoesSocketExist(InSocketName))
            {
                UE_LOG(LogTemp, Warning, TEXT("Socket '%s' does not exist on static mesh component"), *InSocketName.ToString());
                return bAttached;
            }
        }
        SocketName = InSocketName;
        bAttached = AttachToComponent(ParentComponent, FAttachmentTransformRules::KeepRelativeTransform, InSocketName);
    }

    return bAttached;
}


void UAC_LineTrace::PerformLineTrace()
{
    FHitResult HitResult;
    FCollisionQueryParams Params;

    FVector StartLocation = this->GetComponentLocation();
    FVector EndLocation = this->GetComponentLocation() + (this->GetForwardVector() * 1000.0f);

    Params.AddIgnoredActor(GetOwner());
        
    bool bHit = GetWorld()->LineTraceSingleByChannel(
        HitResult,
        StartLocation,
        EndLocation,
        ECC_Visibility,
        Params
    );

    // DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 5.0f, 0, 1.0f);

    if (bHit && HitResult.GetActor())
    {
        OnHitDetected.Broadcast(HitResult.GetActor());
    }
}