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
		PossessedCharacter->SetThisPlayerController(this);
	}
}

void AFPCharacterPlayerController::OnCharacterDeath()
{
	UE_LOG(LogTemp, Warning, TEXT("Received!"))
}
