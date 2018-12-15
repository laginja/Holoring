// Fill out your copyright notice in the Description page of Project Settings.

#include "HoloringGameInstance.h"

#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "MenuSystem/MainMenu.h"

UHoloringGameInstance::UHoloringGameInstance(const FObjectInitializer & ObjectInitializer)
{
	// Get BP widget class
	ConstructorHelpers::FClassFinder<UUserWidget> MenuBPClass(TEXT("/Game/WBP_MainMenu"));

	if (!ensure(MenuBPClass.Class != nullptr)) return;
	MenuClass = MenuBPClass.Class;
}

void UHoloringGameInstance::LoadMenuWidget()
{
	if (!ensure(MenuClass != nullptr)) return;

	Menu = CreateWidget<UMainMenu>(this, MenuClass);
	if (!ensure(Menu != nullptr)) return;

	Menu->Setup();
	// TODO; Menu->SetMenuInterface();

}
