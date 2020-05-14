// Fill out your copyright notice in the Description page of Project Settings.


#include "Lobby.h"

#include "Components/Button.h"

bool ULobby::Initialize()
{
	bool Success = Super::Initialize();
	if (!Success) return false;

	if (!ensure(StartGameButton != nullptr)) return false;
	StartGameButton->OnClicked.AddDynamic(this, &ULobby::StartGame);

	return true;
}

void ULobby::StartGame()
{
	GameInstance->StartGame();
}
