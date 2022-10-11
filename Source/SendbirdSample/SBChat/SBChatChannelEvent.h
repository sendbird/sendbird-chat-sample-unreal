// Copyright (c) 2021 Sendbird, Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SBChatCommonStruct.h"
#include "SBChatChannelEvent.generated.h"

UINTERFACE(BlueprintType, MinimalAPI)
class USBChatChannelEvent : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class SENDBIRDSAMPLE_API ISBChatChannelEvent
{
	GENERATED_IINTERFACE_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "SBChatChannelEvent")
	void OnMessageReceived(const FSBMessageInfo& MessageInfo);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "SBChatChannelEvent")
	void OnMessageDeleted(int64 MessageID);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "SBChatChannelEvent")
	void OnMessageUpdated(const FSBMessageInfo& MessageInfo);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "SBChatChannelEvent")
	void OnUserJoined(const FSBUserInfo& UserInfo);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "SBChatChannelEvent")
	void OnUserLeft(const FSBUserInfo& UserInfo);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "SBChatChannelEvent")
	void OnUserEntered(const FSBUserInfo& UserInfo);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "SBChatChannelEvent")
	void OnUserExited(const FSBUserInfo& UserInfo);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "SBChatChannelEvent")
	void OnInvitationReceived(const FSBChannelInfo& ChannelInfo, const TArray<FSBUserInfo>& UserInfos);
};
