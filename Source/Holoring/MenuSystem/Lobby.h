// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuSystem/MenuWidget.h"
#include "Lobby.generated.h"

/**
 * 
 */
UCLASS()
class HOLORING_API ULobby : public UMenuWidget
{
	GENERATED_BODY()
	
protected:
	virtual bool Initialize() override;

private:
	UPROPERTY(meta = (BindWidget))
	class UButton* StartGameButton;

	// Join an active session
	UFUNCTION()
	void StartGame();
};
