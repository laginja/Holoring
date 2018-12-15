// Fill out your copyright notice in the Description page of Project Settings.

#include "MainMenu.h"

#include "Components/Button.h"

bool UMainMenu::Initialize()
{
	bool Success = Super::Initialize();
	if (!Success) return false;

	if (!ensure(HostButton != nullptr)) return false;
	HostButton->OnClicked.AddDynamic(this, &UMainMenu::HostServer);

	if (!ensure(JoinButton != nullptr)) return false;
	JoinButton->OnClicked.AddDynamic(this, &UMainMenu::OpenJoinMenu);

	return true;
}

void UMainMenu::HostServer()
{
	FString SessionName = "Test sesija";
	GameInstance->Host(SessionName);
}

void UMainMenu::OpenJoinMenu()
{
	if (!ensure(JoinMenu != nullptr)) return;

	GameInstance->RefreshServerList();
}
