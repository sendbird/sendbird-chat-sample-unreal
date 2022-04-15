// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "SendbirdSampleGameMode.generated.h"

UCLASS(MinimalAPI)
class ASendbirdSampleGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ASendbirdSampleGameMode();
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;	
	virtual void NotifyPendingConnectionLost(const FUniqueNetIdRepl& ConnectionUniqueId) override;
	virtual void HandleDisconnect(UWorld* InWorld, UNetDriver* NetDriver) override;

	UPROPERTY()
	TMap<FString, UTexture2DDynamic*> CachedProfileTexture;
};
