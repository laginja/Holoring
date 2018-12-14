// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "HoloringGameMode.h"
#include "HoloringHUD.h"
#include "HoloringCharacter.h"
#include "UObject/ConstructorHelpers.h"

AHoloringGameMode::AHoloringGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AHoloringHUD::StaticClass();
}
