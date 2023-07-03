// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "MultiplayerSessionSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMultiplayerSessionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UMultiplayerSessionSubsystem();

	// ----------------------------------------------------------------
	// To handle session functionality. The Menu class will call these 
	// ----------------------------------------------------------------
	void CreateSession(int32 NumPublicConnections, FString MatchType);
	void FindSession(int32 MaxSearchResults);
	void JoinSession(const FOnlineSessionSearchResult& SessionResult);
	void StartSession();
	void DestroySession();

protected:
	// ----------------------------------------------------------------------------------------------
	// Internal Callbacks for the delegate we'll add to the Online Session Interface delegate list.
	// These don't need to be called outside this class. 
	//-----------------------------------------------------------------------------------------------
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnStartSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);


private:
	IOnlineSessionPtr SessionInterface;

	// ------------------------------------------------------------------------
	// To add to the Onlie Session Interface delegate list.
	// We'll bind our MultiplayersessionSubsystem internal callbacks to these.
	// ------------------------------------------------------------------------
	FOnCreateSessionCompleteDelegate	CreateSessionCompleteDelegate;
	FDelegateHandle						CreateSessionCompleteDelegateHandle;
	FOnFindSessionsCompleteDelegate		FindSessionCompleteDelegate;
	FDelegateHandle						FindSessionCompleteDelegateHandle;
	FOnJoinSessionCompleteDelegate		JoinSessionCompleteDelegate;
	FDelegateHandle						JoinSessionCompleteDelegateHandle;
	FOnStartSessionCompleteDelegate		StartSessionCompletetDelegate;
	FDelegateHandle						StartSessionCompleteDelegateHandle;
	FOnDestroySessionCompleteDelegate	DestroySessionCompleteDelegate;
	FDelegateHandle						DestroySessionCompleteDelegateHandle;
};
