// Fill out your copyright notice in the Description page of Project Settings.

#include "HoloringGameInstance.h"

#include "Engine/Engine.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "MenuSystem/MainMenu.h"
#include "MenuSystem/EndGameMenu.h"
#include "MenuSystem/Lobby.h"
#include "OnlineSessionSettings.h"

const static FName SESSION_NAME = NAME_GameSession;
const static FName SERVER_NAME_SETTINGS_KEY = TEXT("ServerName");

UHoloringGameInstance::UHoloringGameInstance(const FObjectInitializer & ObjectInitializer)
{
	// Get BP widget class
	ConstructorHelpers::FClassFinder<UUserWidget> MenuBPClass(TEXT("/Game/Menu/WBP_MainMenu"));

	ConstructorHelpers::FClassFinder<UUserWidget> EndGameMenuBPClass(TEXT("/Game/Menu/WBP_EndGameMenu"));

	ConstructorHelpers::FClassFinder<UUserWidget> LobbyBPClass(TEXT("/Game/Menu/WBP_Lobby"));

	if (!ensure(MenuBPClass.Class != nullptr)) return;
	MenuClass = MenuBPClass.Class;

	if (!ensure(EndGameMenuBPClass.Class != nullptr)) return;
	EndGameMenuClass = EndGameMenuBPClass.Class;

	if (!ensure(LobbyBPClass.Class != nullptr)) return;
	LobbyClass = LobbyBPClass.Class;
}

void UHoloringGameInstance::Init()
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem != __nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Found subsystem: %s"), *Subsystem->GetSubsystemName().ToString());
		
		SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UHoloringGameInstance::OnCreateSessionComplete);
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UHoloringGameInstance::OnDestroySessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UHoloringGameInstance::OnFindSessionsComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UHoloringGameInstance::OnJoinSessionComplete);

		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No subsystem found"));
	}
}

void UHoloringGameInstance::LoadMenuWidget()
{
	if (!ensure(MenuClass != nullptr)) return;

	Menu = CreateWidget<UMainMenu>(this, MenuClass);
	if (!ensure(Menu != nullptr)) return;

	Menu->Setup();
	Menu->SetGameInstance(this);
}

void UHoloringGameInstance::LoadEndGameMenuWidget()
{
	if (!ensure(EndGameMenuClass != nullptr)) return;

	EndGameMenu = CreateWidget<UEndGameMenu>(this, EndGameMenuClass);
	if (!ensure(EndGameMenu != nullptr)) return;

	EndGameMenu->Setup();
	EndGameMenu->SetGameInstance(this);
}

void UHoloringGameInstance::LoadLobbyWidget()
{
	if (!ensure(LobbyClass != nullptr)) return;

	LobbyMenu = CreateWidget<ULobby>(this, LobbyClass);
	if (!ensure(LobbyMenu != nullptr)) return;

	LobbyMenu->Setup();
	LobbyMenu->SetGameInstance(this);
}

void UHoloringGameInstance::Host(FString DesiredServerName)
{
	ServerName = DesiredServerName;
	//UE_LOG(LogTemp, Warning, TEXT("Hosting a server: %s"), *SessionName);
	if (SessionInterface.IsValid())
	{
		FNamedOnlineSession* ExistingSession = SessionInterface->GetNamedSession(SESSION_NAME);
		if (ExistingSession != nullptr)
		{
			SessionInterface->DestroySession(SESSION_NAME);
			CreateSession();
		}
		else
		{
			CreateSession();
		}
	}
}

void UHoloringGameInstance::CreateSession()
{
	if (SessionInterface.IsValid())
	{
		FOnlineSessionSettings SessionSettings;
		if (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL")
		{
			SessionSettings.bIsLANMatch = true;
		}
		else
		{
			SessionSettings.bIsLANMatch = false;
		}
		SessionSettings.NumPublicConnections = 5;
		SessionSettings.bShouldAdvertise = true;
		SessionSettings.bUsesPresence = true;		// This should be true if not connected over LAN
		SessionSettings.Set(SERVER_NAME_SETTINGS_KEY, ServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
		SessionInterface->CreateSession(0, SESSION_NAME, SessionSettings);
	}
}

void UHoloringGameInstance::DestroySession()
{
	if (SessionInterface.IsValid())
	{
		FNamedOnlineSession* ExistingSession = SessionInterface->GetNamedSession(SESSION_NAME);
		if (ExistingSession != nullptr)
		{
			SessionInterface->DestroySession(SESSION_NAME);
		}
	}
}

void UHoloringGameInstance::OnCreateSessionComplete(FName SessionName, bool Success)
{
	if (!Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not create session"));
		return;
	}
	if (Menu != nullptr)
	{
		Menu->Teardown();
	}

	UEngine* Engine = GetEngine();
	if (!ensure(Engine != nullptr)) return;
	Engine->AddOnScreenDebugMessage(0, 5.f, FColor::Red, TEXT("Hosting"));

	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;
	World->ServerTravel("/Game/Maps/Lobby?listen");
}

void UHoloringGameInstance::OnDestroySessionComplete(FName SessionName, bool Success)
{
	if (Success)
	{
		//CreateSession();
		UE_LOG(LogTemp, Warning, TEXT("Session destroyed!"));
	}
}

void UHoloringGameInstance::StartGame()
{
	if (LobbyMenu != nullptr)
	{
		LobbyMenu->Teardown();
	}

	UEngine* Engine = GetEngine();
	if (!ensure(Engine != nullptr)) return;
	Engine->AddOnScreenDebugMessage(0, 5.f, FColor::Red, TEXT("Started a game"));

	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;
	World->ServerTravel("/Game/Maps/MainLevel/MainLevel?listen");
}

void UHoloringGameInstance::RefreshServerList()
{
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	if (SessionSearch.IsValid())
	{
		SessionSearch->MaxSearchResults = 100;
		SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
		UE_LOG(LogTemp, Warning, TEXT("Searching for sessions"));
		SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
	}
}

void UHoloringGameInstance::OnFindSessionsComplete(bool Success)
{
	if (Success && SessionSearch.IsValid() && Menu != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Finished finding sessions"));
		auto SearchResults = SessionSearch->SearchResults;

		TArray<FServerData> ServerNames;

		if (SearchResults.Num() > 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("Found Active sessions"));
			for (FOnlineSessionSearchResult& SearchResult : SearchResults)
			{
				UE_LOG(LogTemp, Warning, TEXT("Session ID: %s"), *SearchResult.GetSessionIdStr());
				FServerData Data;

				Data.HostUsername = SearchResult.Session.OwningUserName;
				Data.MaxPlayers = SearchResult.Session.SessionSettings.NumPublicConnections;
				Data.CurrentPlayers = Data.MaxPlayers - SearchResult.Session.NumOpenPublicConnections;			// MAX connections number - FREE connections number
				FString ServerName;
				if (SearchResult.Session.SessionSettings.Get(SERVER_NAME_SETTINGS_KEY, ServerName))
				{
					Data.Name = ServerName;
				}
				else
				{
					Data.Name = "Could not find server name";
				}
				ServerNames.Add(Data);
			}
			Menu->SetServerList(ServerNames);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No Active sessions found"));
		}
	}
}


void UHoloringGameInstance::Join(uint32 Index)
{
	if (!SessionInterface.IsValid()) return;
	if (!SessionSearch.IsValid()) return;

	if (Menu != nullptr)
	{
		Menu->Teardown();
	}

	SessionInterface->JoinSession(0, SESSION_NAME, SessionSearch->SearchResults[Index]);
}

void UHoloringGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (!SessionInterface.IsValid()) return;

	FString Address;		// OUT param
	if (!SessionInterface->GetResolvedConnectString(SessionName, Address))		// "za ovaj SessionName daj mi njegovu IP adresu"
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not get connect string"));
		return;
	}

	UEngine* Engine = GetEngine();
	if (!ensure(Engine != nullptr)) return;

	Engine->AddOnScreenDebugMessage(0, 5.f, FColor::Red, FString::Printf(TEXT("Joining %s"), *Address));

	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (!ensure(PlayerController != nullptr)) return;

	PlayerController->ClientTravel(*Address, ETravelType::TRAVEL_Absolute);

}

