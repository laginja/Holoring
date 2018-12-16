// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuWidget.h"
#include "MainMenu.generated.h"

USTRUCT()
struct FServerData
{
	GENERATED_BODY()

	FString Name;
	uint16 CurrentPlayers;
	uint16 MaxPlayers;
	FString HostUsername;
};

/**
 * 
 */
UCLASS()
class HOLORING_API UMainMenu : public UMenuWidget
{
	GENERATED_BODY()
	
public:
	UMainMenu(const FObjectInitializer & ObjectInitializer);

	void SetServerList(TArray<FServerData> ServerNames);
	void SelectIndex(uint32 Index);

protected:
	virtual bool Initialize() override;

private:
	TSubclassOf<class UUserWidget> ServerRowClass;

	UPROPERTY(meta = (BindWidget))
	class UButton* HostButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* JoinButton;

	/**
	Quit Button
	*/
	UPROPERTY(meta = (BindWidget))
	class UButton* QuitGameButton;

	/**
	Join menu
	*/
	UPROPERTY(meta = (BindWidget))
	class UButton* BackJoinMenuButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* ConfirmJoinMenuButton;
	
	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* MenuSwitcher;

	UPROPERTY(meta = (BindWidget))
	class UPanelWidget* ServerList;

	// Main Menu
	UPROPERTY(meta = (BindWidget))
	class UWidget* MainMenu;

	// Menu for joining active sessions
	UPROPERTY(meta = (BindWidget))
	class UWidget* JoinMenu;

	// Host a server
	UFUNCTION()
	void HostServer();

	// Join an active session
	UFUNCTION()
	void JoinServer();

	// Quits the game
	UFUNCTION()
	void QuitGame();

	// Open Host Menu
	UFUNCTION()
	void OpenHostMenu();

	// Open Join Menu
	UFUNCTION()
	void OpenJoinMenu();

	// Open Main Menu
	UFUNCTION()
	void OpenMainMenu();

	TOptional<uint32> SelectedIndex;

	void UpdateChildren();
};
