// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FPCharacterPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class HOLORING_API AFPCharacterPlayerController : public APlayerController
{
	GENERATED_BODY()
	
private: 
	virtual void SetPawn(APawn* InPawn) override;		// This method is called when the controlles possesses a character. Using it to subscribe to characters OnDeath event.
	
	UFUNCTION()
	void OnCharacterDeath();

	UFUNCTION()
	void OnScoredKill();

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "Player Handling")
	void PlayerDied(AFPCharacterPlayerController* PlayerController);

	UFUNCTION(BlueprintImplementableEvent, Category = "Player Handling")
	void IncreaseScore();
};
