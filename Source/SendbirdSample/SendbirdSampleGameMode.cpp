// Copyright Epic Games, Inc. All Rights Reserved.

#include "SendbirdSampleGameMode.h"
#include "UObject/ConstructorHelpers.h"
#include "SendbirdChatManager.h"

ASendbirdSampleGameMode::ASendbirdSampleGameMode()
{
}

void ASendbirdSampleGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	CachedProfileTexture.Reset();
	SendbirdChatMananger::Get().Reset();
}

void ASendbirdSampleGameMode::NotifyPendingConnectionLost(const FUniqueNetIdRepl& ConnectionUniqueId)
{
}

void ASendbirdSampleGameMode::HandleDisconnect(UWorld* InWorld, UNetDriver* NetDriver)
{
}
