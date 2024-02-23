// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#if WITH_SENDBIRD
#include "Sendbird.h"
#endif
#include "SendbirdWrapper.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FConnectDelegate, bool, result, const FString&, userId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDisconnectDelegate);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FReconnectionStartedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FReconnectionSucceededDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FReconnectionFailedDelegate);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FGroupChannelUserMessageReceivedDelegate, const FString&, channelUrl, const FString&, message, const FString&, sender);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOpenChannelUserMessageReceivedDelegate, const FString&, channelUrl, const FString&, message, const FString&, sender);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FGroupChannelListQueryDelegate, bool, result, const TArray<FString>&, channelUrls, const TArray<FString>&, channelNames, const TArray<int>&, memberCounts);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGroupChannelCreateDelegate, bool, result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FGroupChannelMessageListQueryDelegate, bool, result, const TArray<FString>&, messages, const TArray<FString>&, senders);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FGroupChannelSendUserMessageDelegate, bool, result, const FString&, message, const FString&, sender);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOpenChannelListQueryDelegate, bool, result, const TArray<FString>&, channelUrls, const TArray<FString>&, channelNames, const TArray<int>&, participantCounts);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOpenChannelCreateDelegate, bool, result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOpenChannelMessageListQueryDelegate, bool, result, const TArray<FString>&, messages, const TArray<FString>&, senders);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOpenChannelEnterDelegate, bool, result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOpenChannelExitDelegate, bool, result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOpenChannelSendUserMessageDelegate, bool, result, const FString&, message, const FString&, sender);


#if WITH_SENDBIRD
class ChannelHandler : public SBDChannelHandler {
public:
	ChannelHandler(USendbirdWrapper* sendbirdWrapper) : sendbird(sendbirdWrapper) {}
	virtual ~ChannelHandler() {}

private:
	void MessageReceived(SBDBaseChannel* channel, SBDBaseMessage* message);

private:
	TWeakObjectPtr<USendbirdWrapper> sendbird;
};

class ReconnectionHandler : public SBDConnectionHandler {
public:
	ReconnectionHandler(USendbirdWrapper* sendbirdWrapper) : sendbird(sendbirdWrapper) {}
	virtual ~ReconnectionHandler() {}

private:
	void Started();
	void Succeeded();
	void Failed();

private:
	TWeakObjectPtr<USendbirdWrapper> sendbird;
};
#endif


/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class SAMPLE_API USendbirdWrapper : public UObject
{
	GENERATED_BODY()

public:
	USendbirdWrapper();
	~USendbirdWrapper();


#if WITH_SENDBIRD
	TSharedPtr<ChannelHandler> channelHandler;
	TSharedPtr<ReconnectionHandler> reconnectionHandler;
#endif


	UFUNCTION(BlueprintCallable, Category="Sendbird")
	FString GetSdkVersion();


	// Connection
	UPROPERTY(BlueprintAssignable, VisibleAnyWhere, BlueprintCallable)
	FConnectDelegate connectDelegate;
	UFUNCTION(BlueprintCallable, Category="Sendbird")
	void Connect(const FString& userId);
	
	UPROPERTY(BlueprintAssignable, VisibleAnyWhere, BlueprintCallable)
	FDisconnectDelegate disconnectDelegate;
	UFUNCTION(BlueprintCallable, Category= "Sendbird")
	void Disconnect();


	// Reconnection
	UFUNCTION(BlueprintCallable, Category="Sendbird")
	void AddReconnectionHandler();
	UPROPERTY(BlueprintAssignable, VisibleAnyWhere, BlueprintCallable)
	FReconnectionStartedDelegate reconnectionStartedDelegate;
	UPROPERTY(BlueprintAssignable, VisibleAnyWhere, BlueprintCallable)
	FReconnectionSucceededDelegate reconnectionSucceededDelegate;
	UPROPERTY(BlueprintAssignable, VisibleAnyWhere, BlueprintCallable)
	FReconnectionFailedDelegate reconnectionFailedDelegate;


	// ChannelHandler
	UFUNCTION(BlueprintCallable, Category="Sendbird")
	void AddChannelHandler();
	
	UPROPERTY(BlueprintAssignable, VisibleAnyWhere, BlueprintCallable)
	FGroupChannelUserMessageReceivedDelegate groupChannelUserMessageReceivedDelegate;
	void GroupChannelUserMessageReceived(const FString& channelUrl, const FString& message, const FString& sender);

	UPROPERTY(BlueprintAssignable, VisibleAnyWhere, BlueprintCallable)
	FOpenChannelUserMessageReceivedDelegate openChannelUserMessageReceivedDelegate;
	void OpenChannelUserMessageReceived(const FString& channelUrl, const FString& message, const FString& sender);


	// GroupChannel
	UPROPERTY(BlueprintAssignable, VisibleAnyWhere, BlueprintCallable)
	FGroupChannelListQueryDelegate groupChannelListQueryDelegate;
	UFUNCTION(BlueprintCallable, Category= "Sendbird")
	void GroupChannelListQuery();

	UPROPERTY(BlueprintAssignable, VisibleAnyWhere, BlueprintCallable)
	FGroupChannelCreateDelegate groupChannelCreateDelegate;
	UFUNCTION(BlueprintCallable, Category= "Sendbird")
	void GroupChannelCreate(const FString& channelName, const FString& userIdToInvite);
	
	UPROPERTY(BlueprintAssignable, VisibleAnyWhere, BlueprintCallable)
	FGroupChannelMessageListQueryDelegate groupChannelMessageListQueryDelegate;
	UFUNCTION(BlueprintCallable, Category= "Sendbird")
	void GroupChannelMessageListQuery(const FString& channelUrl);

	UPROPERTY(BlueprintAssignable, VisibleAnyWhere, BlueprintCallable)
	FGroupChannelSendUserMessageDelegate groupChannelSendUserMessageDelegate;
	UFUNCTION(BlueprintCallable, Category= "Sendbird")
	void GroupChannelSendUserMessage(const FString& channelUrl, const FString&  message);

	
	// OpenChannel
	UPROPERTY(BlueprintAssignable, VisibleAnyWhere, BlueprintCallable)
	FOpenChannelListQueryDelegate openChannelListQueryDelegate;
	UFUNCTION(BlueprintCallable, Category= "Sendbird")
	void OpenChannelListQuery();

	UPROPERTY(BlueprintAssignable, VisibleAnyWhere, BlueprintCallable)
	FOpenChannelCreateDelegate openChannelCreateDelegate;
	UFUNCTION(BlueprintCallable, Category= "Sendbird")
	void OpenChannelCreate(const FString& channelName);
	
	UPROPERTY(BlueprintAssignable, VisibleAnyWhere, BlueprintCallable)
	FOpenChannelMessageListQueryDelegate openChannelMessageListQueryDelegate;
	UFUNCTION(BlueprintCallable, Category= "Sendbird")
	void OpenChannelMessageListQuery(const FString& channelUrl);

	UPROPERTY(BlueprintAssignable, VisibleAnyWhere, BlueprintCallable)
	FOpenChannelEnterDelegate openChannelEnterDelegate;
	UFUNCTION(BlueprintCallable, Category= "Sendbird")
	void OpenChannelEnter(const FString& channelUrl);

	UPROPERTY(BlueprintAssignable, VisibleAnyWhere, BlueprintCallable)
	FOpenChannelExitDelegate openChannelExitDelegate;
	UFUNCTION(BlueprintCallable, Category= "Sendbird")
	void OpenChannelExit(const FString& channelUrl);

	UPROPERTY(BlueprintAssignable, VisibleAnyWhere, BlueprintCallable)
	FOpenChannelSendUserMessageDelegate openChannelSendUserMessageDelegate;
	UFUNCTION(BlueprintCallable, Category= "Sendbird")
	void OpenChannelSendUserMessage(const FString& channelUrl, const FString&  message);
};
