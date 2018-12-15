// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "HoloringGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class HOLORING_API UHoloringGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UHoloringGameInstance(const FObjectInitializer & ObjectInitializer);
	
	UFUNCTION(BlueprintCallable)
	void LoadMenuWidget();
	
private:
	TSubclassOf<class UUserWidget> MenuClass;
	class UMainMenu* Menu;
};
