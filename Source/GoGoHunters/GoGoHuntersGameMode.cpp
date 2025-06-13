// Copyright Epic Games, Inc. All Rights Reserved.

#include "GoGoHuntersGameMode.h"
#include "GoGoHuntersCharacter.h"
#include "UObject/ConstructorHelpers.h"

AGoGoHuntersGameMode::AGoGoHuntersGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
