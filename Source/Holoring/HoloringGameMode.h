// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HoloringGameMode.generated.h"

UCLASS(minimalapi)
class AHoloringGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AHoloringGameMode();

	UFUNCTION(BlueprintCallable)
	void LoadGameEndLevel();
};



