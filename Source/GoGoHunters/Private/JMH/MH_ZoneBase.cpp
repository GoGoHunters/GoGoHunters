// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_ZoneBase.h"

#include "Components/SphereComponent.h"
#include "JMH/MH_VRPlayer.h"

// Sets default values
AMH_ZoneBase::AMH_ZoneBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 콜리전 판정용 스피어
	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	RootComponent = CollisionSphere;

	CollisionSphere->InitSphereRadius(400.f);
	CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionSphere->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionSphere->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	// 시각적 표시용 메쉬
	ZoneVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ZoneVisual"));
	ZoneVisual->SetupAttachment(RootComponent);
	ZoneVisual->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void AMH_ZoneBase::BeginPlay()
{
	Super::BeginPlay();
	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AMH_ZoneBase::OnZoneOverlapBegin);
}

// Called every frame
void AMH_ZoneBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMH_ZoneBase::OnPlayerInteracted_Implementation(AActor* Player)
{
	if (!GuideMessage.IsEmpty())
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, GuideMessage);
	}

	static const TMap<FName, TFunction<void(AMH_ZoneBase*, AActor*)>> ZoneFunctionMap = {
		{"Globe", [](AMH_ZoneBase* Z, AActor* P) { Z->HandleGlobeInteraction(P); }},
		{"Restore", [](AMH_ZoneBase* Z, AActor* P) { Z->HandleRestoreInteraction(P); }},
		{"Museum", [](AMH_ZoneBase* Z, AActor* P) { Z->HandleMyMuseumInteraction(P); }},
		{"Record", [](AMH_ZoneBase* Z, AActor* P) { Z->HandleRecordInteraction(P); }},
		{"Settings", [](AMH_ZoneBase* Z, AActor* P) { Z->HandleSettingsInteraction(P); }},
		{"Exit", [](AMH_ZoneBase* Z, AActor* P) { Z->HandleExitInteraction(P); }}
	};

	if (const TFunction<void(AMH_ZoneBase*, AActor*)>* Func = ZoneFunctionMap.Find(ZoneTag))
	{
		(*Func)(this, Player);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("[ZoneBase] Unknown ZoneTag"));
	}
}

void AMH_ZoneBase::OnZoneOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor) return;

	// 플레이어일 경우에만 실행 (플레이어 클래스 체크)
	if (OtherActor->IsA(AMH_VRPlayer::StaticClass()))
	{
		OnPlayerInteracted(OtherActor);  // 지금 구조에서는 그냥 호출 (혹은 나중엔 Player 전달 가능)
	}
}

void AMH_ZoneBase::HandleGlobeInteraction(AActor* Player)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, TEXT("[Globe] 탐험지역 UI 열림"));
	// TODO: 탐험 UI 위젯 열기
}

void AMH_ZoneBase::HandleRestoreInteraction(AActor* Player)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("[Restore] 유물 복원 UI 실행"));
	// TODO: 복원 미니게임 실행
}

void AMH_ZoneBase::HandleMyMuseumInteraction(AActor* Player)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Magenta, TEXT("[Museum] 유물 전시 UI 열림"));
	// TODO: 전시 기능 위젯 실행
}

void AMH_ZoneBase::HandleRecordInteraction(AActor* Player)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Silver, TEXT("[Record] 탐험 기록 열람 UI 표시"));
	// TODO: 도감/기록 확인 UI 띄우기
}

void AMH_ZoneBase::HandleSettingsInteraction(AActor* Player)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::White, TEXT("[Settings] 설정 메뉴 실행"));
	// TODO: 설정 UI 열기
}

void AMH_ZoneBase::HandleExitInteraction(AActor* Player)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::White, TEXT("[Exit] Exit 메뉴 실행"));
	// TODO: Exit UI 열기
}
