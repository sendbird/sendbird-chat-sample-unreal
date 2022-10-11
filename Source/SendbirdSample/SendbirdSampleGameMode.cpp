// Copyright Epic Games, Inc. All Rights Reserved.

#include "SendbirdSampleGameMode.h"
#include "UObject/ConstructorHelpers.h"
#include "SBChat/SBChatManager.h"

ASendbirdSampleGameMode::ASendbirdSampleGameMode()
{
}

void ASendbirdSampleGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	SBChatManager::Get().Reset();
}
