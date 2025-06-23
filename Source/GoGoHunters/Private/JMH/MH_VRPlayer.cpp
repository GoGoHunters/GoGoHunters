// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_VRPlayer.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "IXRTrackingSystem.h"
#include "Camera/CameraComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"


// Sets default values
AMH_VRPlayer::AMH_VRPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	VRCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("VRCamera"));
	VRCamera->SetupAttachment(RootComponent);

	TeleportCircleA = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TeleportCircle"));
	TeleportCircleA->SetupAttachment(RootComponent);

	TeleportUIComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TeleportUIComponent"));
	TeleportUIComponent->SetupAttachment(RootComponent);

	//Attachement 나중에 RootComp로 바꿔야함 /수정
	L_Hand = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("L_Hand"));
	L_Hand->SetupAttachment(VRCamera);
	R_Hand = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("R_Hand"));
	R_Hand->SetupAttachment(VRCamera);
}

// Called when the game starts or when spawned
void AMH_VRPlayer::BeginPlay()
{
	Super::BeginPlay();
//텔레포트 초기화
	ResetTeleport();

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InputMappingContext, 0);
		}
	}
}

// Called every frame
void AMH_VRPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//텔레포트 활성화 시
	if (bTeleporting)
	{
		//텔레포트 그리기 곡선방식
		if (bTeleportCurve)
		{
			DrawTeleportCurve();
		}
		else
		{
			DrawTeleportStraight();
		}

		UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector(TeleportUIComponent,TEXT("User.PointArray"),Lines);
	}

}

// Called to bind functionality to input
void AMH_VRPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	
	EnhancedInput->BindAction(IA_MHTurn,ETriggerEvent::Triggered,this,&AMH_VRPlayer::TestTurn);
	EnhancedInput->BindAction(IA_MHLookUp,ETriggerEvent::Triggered,this,&AMH_VRPlayer::TestLookUp);
	EnhancedInput->BindAction(IA_MHInteract,ETriggerEvent::Triggered,this,&AMH_VRPlayer::TestInteract);
	EnhancedInput->BindAction(IA_MHTeleportEnd,ETriggerEvent::Triggered,this,&AMH_VRPlayer::F_TeleportEnd);
	EnhancedInput->BindAction(IA_MHTeleportStart,ETriggerEvent::Triggered,this,&AMH_VRPlayer::F_TeleportStart);

}

bool AMH_VRPlayer::ResetTeleport()
{
	//현재 텔레포트 서클이 보여지고 있으면 이동가능
	//그렇지 않으면 이동 불가능
	bool bCanTeleport = TeleportCircleA->GetVisibleFlag();
	//텔레포트 종료
	bTeleporting = false;
	TeleportCircleA->SetVisibleFlag(false);
	TeleportUIComponent->SetVisibility(false);
	//텔레포트 가능여부 결과로 넘겨줌
	return bCanTeleport;
}

bool AMH_VRPlayer::CheckHitTeleport(FVector LastPos, FVector& CurPos)
{
	FHitResult outHit;
	FCollisionQueryParams query;
	query.AddIgnoredActor(this);
	bool bHit = GetWorld()->LineTraceSingleByChannel(outHit,LastPos,CurPos,ECC_Visibility,query);
	//3.Line과 부딪혔다면
	AActor* HitActor = outHit.GetActor();
	//4.그리고 부딫힌 액터이름이 Floor라면
	if (bHit && HitActor && HitActor->GetActorNameOrLabel().Contains("Floor"))
	{
		//텔레포트 UI활성화
		TeleportCircleA->SetVisibility(true);
		//->TeleportCircle 위치시키기
		TeleportCircleA->SetWorldLocation(outHit.Location);

		//텔레포트 위치 지정
		TeleportLocation = outHit.Location;
		CurPos = TeleportLocation;
		bCanTeleportLocationValid = true;
	}
	//5. 안부딫혔으면
	else
	{
		//-> TeleportCircle 안그려지게 하기
		TeleportCircleA->SetVisibility(false);
		bCanTeleportLocationValid = false;
	}
	return bHit;
}

void AMH_VRPlayer::DrawTeleportStraight()
{
	//Line Trace
	//1.Line 만들기
	FVector StartPoint = R_Hand->GetComponentLocation();
	FVector EndPoint = StartPoint + R_Hand->GetForwardVector() * 1000;

	bool bHit = CheckHitTeleport(StartPoint,EndPoint);

	Lines.Empty();
	Lines.Add(StartPoint);
	Lines.Add(EndPoint);

	/*
	if (bIsDebugDraw)
	{
		//선그리기
		DrawDebugLine(GetWorld(),StartPoint,EndPoint,FColor::Red,false,-1,0,1);
	}*/
}

void AMH_VRPlayer::DrawTeleportCurve()
{
	Lines.Empty();

	//선이 진행될 힘(방향)
	FVector velocity = R_Hand->GetForwardVector() * CurveForce;

	//P0 - 시작점
	FVector pos = R_Hand->GetComponentLocation();
	Lines.Add(pos);

	//FMath::GetReflectionVector()입사각 반사각 계산

	//이 과정을 LineSmooth를 구성하는 (점의 개수 -1)만큼 진행하겠다.
	for (int i = 0; i < LineSmooth; i++)
	{
		FVector LastPos = pos;

		//v = v0 + at
		velocity += FVector::UpVector * Gravity * SimulateTime;

		//P= P0+vt
		pos += velocity * SimulateTime;

		bool bHit = CheckHitTeleport(LastPos,pos);
		Lines.Add(pos);

		//부딪혔을 때 반복중단
		if (bHit)
		{
			break;
		}
		
	}

	//Line을 그려준다
	int LineCount = Lines.Num();
	for (int i = 0; i < LineCount-1; i++)
	{
		DrawDebugLine(GetWorld(),Lines[i],Lines[i + 1],FColor::Red,false,-1,0,1);
	}
	
}

void AMH_VRPlayer::TestTurn(const FInputActionValue& Value)
{
	if (!bUseMouse)
	{
		return;
	}
	float AxisValue = Value.Get<float>();
	AddControllerYawInput(AxisValue);
}

void AMH_VRPlayer::F_TeleportEnd(const struct FInputActionValue& Value)
{
	if (!ResetTeleport())
	{
		return;
	}
	SetActorLocation(TeleportLocation);
	bTeleporting = false;
}

void AMH_VRPlayer::ActiveDebugDraw()
{
	bIsDebugDraw = !bIsDebugDraw;
}

void AMH_VRPlayer::F_TeleportStart(const struct FInputActionValue& Value)
{
	TeleportUIComponent->SetVisibility(true);
	bTeleporting = true;
}

void AMH_VRPlayer::TestInteract()
{
	GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,TEXT("Interact"));
}

void AMH_VRPlayer::TestLookUp(const FInputActionValue& Value)
{
	float AxisValue = Value.Get<float>();
	//AddControllerPitchInput(AxisValue);

	
	// VR 테스트 모드일 때만 적용 (예: HMD 미착용)
	if (!GEngine->XRSystem.IsValid() || !GEngine->XRSystem->IsHeadTrackingAllowed())
	{
		// 마우스 상하를 직접 VRCamera에 적용
		FRotator NewRot = VRCamera->GetRelativeRotation();
		NewRot.Pitch = FMath::Clamp(NewRot.Pitch + AxisValue, -80.f, 80.f);
		VRCamera->SetRelativeRotation(NewRot);
	}
}


