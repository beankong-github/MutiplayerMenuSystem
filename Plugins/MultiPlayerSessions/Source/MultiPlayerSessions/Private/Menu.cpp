// Fill out your copyright notice in the Description page of Project Settings.
#include "Menu.h"
#include "Components/Button.h"
#include "MultiplayerSessionSubsystem.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"

void UMenu::MenuSetup(int32 NumberOfPublicConnections, FString TypeOfMatch, FString LobbyPath)
{
	NumPublicConnections = NumberOfPublicConnections;
	MatchType = TypeOfMatch;
	PathToLevel = FString::Printf(TEXT("%s?listen"), *LobbyPath);


	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	SetIsFocusable(true);

	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController)
		{
			FInputModeUIOnly InputModeData;
			InputModeData.SetWidgetToFocus(TakeWidget());
			InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(true);
		}
	}

	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		MultiplayerSessionSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionSubsystem>();
	}

	// Binding callbacks to Delegates of the MultiplayerSessionSubsystem class
	if (MultiplayerSessionSubsystem)
	{
		MultiplayerSessionSubsystem->MultiplayerOnCreateSessionComplete.AddDynamic(this, &ThisClass::OnCreateSession);
		MultiplayerSessionSubsystem->MultiplayerOnFindSessionComplete.AddUObject(this, &ThisClass::OnFindSession);
		MultiplayerSessionSubsystem->MultiplayerOnJoinSessionComplete.AddUObject(this, &ThisClass::OnJoinSession);
		MultiplayerSessionSubsystem->MultiplayerOnStartSessionComplete.AddDynamic(this, &ThisClass::OnStartSession);
		MultiplayerSessionSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &ThisClass::OnDestroySession);
	}
}


bool UMenu::Initialize()
{
	if (!Super::Initialize())
		return false;

	// Binding Click Callbacks to Buttons 
	if (HostButton)
	{
		HostButton->OnClicked.AddDynamic(this, &ThisClass::HostButtonClicked);
	}
	if (JoinButton)
	{
		JoinButton->OnClicked.AddDynamic(this, &ThisClass::JoinButtonClicked);
	}

	return true;
}

void UMenu::NativeDestruct()
{
	MenuTearDown();
	Super::NativeDestruct();
}

void UMenu::OnCreateSession(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Yellow, FString(TEXT("Session created successfully!")));
		}

		UWorld* World = GetWorld();
		if (World)
		{
			// 세션을 생성하면 Lobby로 이동
			World->ServerTravel(PathToLevel);
		}
	}
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString(TEXT("Failed to create session!")));
		}

		HostButton->SetIsEnabled(true);
	}
}

void UMenu::OnFindSession(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bwasSuccessful)
{
	if (MultiplayerSessionSubsystem == nullptr)
		return;

	for (auto Result : SessionResults)
	{
		// 매치 타입 확인하기
		FString SettingsValue;
		Result.Session.SessionSettings.Get(FName("MatchType"), SettingsValue);

		// 매치 타입이 요건에 맞다면 JoinSession
		if (SettingsValue == MatchType)
		{
			MultiplayerSessionSubsystem->JoinSession(Result);
			return;
		}
	}

	if(!bwasSuccessful || SessionResults.Num() == 0)
		JoinButton->SetIsEnabled(true);
}

void UMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
	// SessionInterface
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem)
	{
		IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();

		if (SessionInterface.IsValid())
		{
			/* Join the Session */
			FString Address;
			if (SessionInterface->GetResolvedConnectString(NAME_GameSession, Address))
			{
				APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
				if (PlayerController)
				{
					PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
				}
			}
		}
	}

	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		JoinButton->SetIsEnabled(true);
	}
}

void UMenu::OnStartSession(bool bWasSuccessful)
{
}

void UMenu::OnDestroySession(bool bWasSuccessful)
{
}


void UMenu::HostButtonClicked()
{
	HostButton->SetIsEnabled(false);
	if (MultiplayerSessionSubsystem)
	{
		MultiplayerSessionSubsystem->CreateSession(NumPublicConnections, MatchType);
	}
}

void UMenu::JoinButtonClicked()
{
	JoinButton->SetIsEnabled(false);
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Yellow, FString(TEXT("JoinButtonClicked!")));
	}

	if (MultiplayerSessionSubsystem)
	{
		MultiplayerSessionSubsystem->FindSession(10'000);
	}
}

void UMenu::MenuTearDown()
{
	RemoveFromParent();
	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController)
		{
			// Input Mode : UIOnly -> GameOnly
			FInputModeGameOnly InputModeData;
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(false);
		}
	}
}
