// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "HoloringGameMode.h"
#include "HoloringHUD.h"
#include "HoloringCharacter.h"
#include "Engine/Engine.h"
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

void AHoloringGameMode::LoadGameEndLevel()
{
	UEngine* Engine = GetGameInstance()->GetEngine();
	if (!ensure(Engine != nullptr)) return;
	Engine->AddOnScreenDebugMessage(0, 5.f, FColor::Red, TEXT("Game Over"));

	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;
	World->ServerTravel("/Game/Maps/GameEndLevel");
}
