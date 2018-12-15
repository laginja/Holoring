// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Holoring/HoloringGameInstance.h"
#include "MenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class HOLORING_API UMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetGameInstance(UHoloringGameInstance* GameInstance);
	
	void Setup();

	void Teardown();

protected:
	UHoloringGameInstance* GameInstance;
};
