// Fill out your copyright notice in the Description page of Project Settings.

#include "FPCharacterPlayerController.h"
#include "HoloringCharacter.h"	// imported so we can implement OnDeath

void AFPCharacterPlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);

	if (InPawn)
	{
		auto PossessedCharacter = Cast<AHoloringCharacter>(InPawn);
		if (!ensure(PossessedCharacter)) { return; }

		// Subscribe our local method to the character's death event
		PossessedCharacter->OnDeath.AddUniqueDynamic(this, &AFPCharacterPlayerController::OnCharacterDeath);
		
		// Subscribe our local method to the when the character scores a kill
		PossessedCharacter->OnKill.AddUniqueDynamic(this, &AFPCharacterPlayerController::OnScoredKill);

		// Forward this controller to the possessed character
		PossessedCharacter->SetThisPlayerController(this);
	}
}

void AFPCharacterPlayerController::OnCharacterDeath()
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *this->GetPawn()->GetName());
	PlayerDied(this);
}

void AFPCharacterPlayerController::OnScoredKill()
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *this->GetPawn()->GetName());
	IncreaseScore();
}

