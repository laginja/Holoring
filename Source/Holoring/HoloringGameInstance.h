// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionInterface.h"
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
	
	virtual void Init();

	UFUNCTION(BlueprintCallable)
	void LoadMenuWidget();
	
	UFUNCTION(Exec)
	void Host(FString DesiredServerName);
	void RefreshServerList();

private:
	void CreateSession();

	void OnCreateSessionComplete(FName SessionName, bool Success);
	void OnDestroySessionComplete(FName SessionName, bool Success);
	void OnFindSessionsComplete(bool Success);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	TSubclassOf<class UUserWidget> MenuClass;
	class UMainMenu* Menu;

	IOnlineSessionPtr SessionInterface;
	FString ServerName;

	// Variable for getting session search results 
	TSharedPtr<class FOnlineSessionSearch> SessionSearch;
};
