// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_ZoneBase.h"
#include "JMH/MH_MessageUI.h"
#include "Components/BoxComponent.h"
#include "JMH/MH_VRPlayer.h"


// Sets default values
AMH_ZoneBase::AMH_ZoneBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 콜리전 판정용 스피어
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionSphere"));
	RootComponent = CollisionBox;
	
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);

	// 시각적 표시용 메쉬
	ZoneVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ZoneVisual"));
	ZoneVisual->SetupAttachment(RootComponent);
	ZoneVisual->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	MessageWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("MessageUI"));
	MessageWidgetComponent->SetupAttachment(RootComponent);

	//현민
	//RestoreListComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("RestoreListUI"));
	//RestoreListComponent->SetupAttachment(RootComponent);
	
}

// Called when the game starts or when spawned
void AMH_ZoneBase::BeginPlay()
{
	Super::BeginPlay();
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AMH_ZoneBase::OnZoneOverlapBegin);
	CollisionBox->OnComponentEndOverlap.AddDynamic(this, &AMH_ZoneBase::OnZoneOverlapEnd);
	
	if (MessageUIClass)
	{
		MessageWidgetComponent->SetWidgetClass(MessageUIClass);

		// BeginPlay 시점에서는 이미 위젯이 만들어져 있을 수 있으므로 캐싱
		MessageUI = Cast<UMH_MessageUI>(MessageWidgetComponent->GetUserWidgetObject());

		if (MessageUI)
		{
			MessageUI->OnCloseClicked.AddDynamic(this, &AMH_ZoneBase::HandleMessageUIClose);
			HandleMessageUIClose();
			MessageUI->SetOuterActor(this);
			MessageUI->SetMessage(FText::FromString(GuideMessage));
			MessageUI->TargetLevel = TargetLevelName;
			MessageUI->ShowButtons(true,false);
		}
	}
	//현민
	/*
	if (RestoreListComponent)
	{
		if (RestoreListUIClass)
		{
			RestoreListComponent->SetWidgetClass(RestoreListUIClass);

			MessageUI = Cast<UMH_MessageUI>(MessageWidgetComponent->GetUserWidgetObject());	
		}
		
	}

	*/
}

// Called every frame
void AMH_ZoneBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMH_ZoneBase::OnPlayerInteracted_Implementation(AActor* Player)
{
	if (!Player) return;

	VRPlayer = Cast<AMH_VRPlayer>(Player);
	if (!VRPlayer) return;
	
	static const TMap<FName, TFunction<void(AMH_ZoneBase*)>> ZoneFunctionMap = {
		{"Restore", [](AMH_ZoneBase* Z) { Z->HandleRestoreInteraction(); }},
		{"Museum", [](AMH_ZoneBase* Z) { Z->HandleMyMuseumInteraction(); }},
		{"Lobby", [](AMH_ZoneBase* Z) { Z->HandleLobbyInteraction(); }},
		{"Exit", [](AMH_ZoneBase* Z) { Z->HandleExitInteraction(); }},
		{"Record", [](AMH_ZoneBase* Z) { Z->HandleRecordInteraction(); }},
		{"RestoreList", [](AMH_ZoneBase* Z) { Z->HandleRestoreListInteraction(); }},	
		//{"Settings", [](AMH_ZoneBase* Z) { Z->HandleSettingsInteraction(); }},
	};

	if (const TFunction<void(AMH_ZoneBase*)>* Func = ZoneFunctionMap.Find(ZoneTag))
	{
		(*Func)(this);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("[ZoneBase] Unknown ZoneTag"));
	}
}

void AMH_ZoneBase::ShowZoneMessageUI(FString Message)
{
	if (!MessageUI) return;
	MessageWidgetComponent->SetVisibility(true);	
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

void AMH_ZoneBase::OnZoneOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor) return;
	
	if (OtherActor->IsA(AMH_VRPlayer::StaticClass()))
	{
		VRPlayer = nullptr;
		HandleMessageUIClose();
	}
}

void AMH_ZoneBase::HandleMessageUIClose()
{
	MessageWidgetComponent->SetVisibility(false);
	//현민
	//RestoreListUI->SetVisibility(false);
}

void AMH_ZoneBase::HandleMapInteraction()
{

}

void AMH_ZoneBase::HandleRestoreInteraction()
{
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("[Restore] 유물 복원 UI 실행"));
	MessageUI->ShowButtons(true,false);
	ShowZoneMessageUI(GuideMessage);
	MessageWidgetComponent->SetVisibility(true);
}

void AMH_ZoneBase::HandleMyMuseumInteraction()
{
	MessageUI->ShowButtons(true,false);
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Magenta, TEXT("[Museum] 유물 전시 UI 열림"));
	ShowZoneMessageUI(GuideMessage);
	MessageWidgetComponent->SetVisibility(true);
}

void AMH_ZoneBase::HandleRecordInteraction()
{
	MessageUI->ShowButtons(true,true);
	ShowZoneMessageUI(GuideMessage);
	MessageWidgetComponent->SetVisibility(true);
}

void AMH_ZoneBase::HandleSettingsInteraction()
{
	
}

void AMH_ZoneBase::HandleExitInteraction()
{
	MessageUI->ShowButtons(true,false);
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::White, TEXT("[Exit] Exit 메뉴 실행"));
	// TODO: Exit UI 열기
	ShowZoneMessageUI(GuideMessage);
	MessageWidgetComponent->SetVisibility(true);
}

void AMH_ZoneBase::HandleLobbyInteraction()
{
	MessageUI->ShowButtons(true,false);
	ShowZoneMessageUI(GuideMessage);
	MessageWidgetComponent->SetVisibility(true);
}

void AMH_ZoneBase::HandleRestoreListInteraction()
{
	//현민
	//RestoreListUI->SetVisibility(true);
	
}
