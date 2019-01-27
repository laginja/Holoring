// Fill out your copyright notice in the Description page of Project Settings.

#include "EndGameMenu.h"
#include "Components/Button.h"

bool UEndGameMenu::Initialize()
{
	bool Success = Super::Initialize();
	if (!Success) return false;

	if (!ensure(QuitGameButton != nullptr)) return false;
	QuitGameButton->OnClicked.AddDynamic(this, &UEndGameMenu::QuitGame);

	if (!ensure(OpenMainMenuButton != nullptr)) return false;
	OpenMainMenuButton->OnClicked.AddDynamic(this, &UEndGameMenu::OpenMainMenu);

	return true;
}

void UEndGameMenu::QuitGame()
{
	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;

	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!ensure(PlayerController != nullptr)) return;

	PlayerController->ConsoleCommand("quit");
}

void UEndGameMenu::OpenMainMenu()
{
	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;

	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!ensure(PlayerController != nullptr)) return;

	PlayerController->ClientReturnToMainMenu("Back to main menu");

	GameInstance->DestroySession();
}
