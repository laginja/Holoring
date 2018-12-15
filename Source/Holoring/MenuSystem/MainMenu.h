// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuWidget.h"
#include "MainMenu.generated.h"

/**
 * 
 */
UCLASS()
class HOLORING_API UMainMenu : public UMenuWidget
{
	GENERATED_BODY()
	
	
protected:
	virtual bool Initialize() override;

private:
	UPROPERTY(meta = (BindWidget))
	class UButton* HostButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* JoinButton;
	
	// Menu for joining active sessions
	UPROPERTY(meta = (BindWidget))
	class UWidget* JoinMenu;

	// Host a server
	UFUNCTION()
	void HostServer();

	// Open Join Menu
	UFUNCTION()
	void OpenJoinMenu();

};
