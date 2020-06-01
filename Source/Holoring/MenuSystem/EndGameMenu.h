// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuSystem/MenuWidget.h"
#include "EndGameMenu.generated.h"

/**
 * 
 */
UCLASS()
class HOLORING_API UEndGameMenu : public UMenuWidget
{
	GENERATED_BODY()
	
protected:
	virtual bool Initialize() override;

private:
	UPROPERTY(meta = (BindWidget))
	class UButton* QuitGameButton;

	// Quits the game
	UFUNCTION()
	void QuitGame();
};
