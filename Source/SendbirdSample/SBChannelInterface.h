// Copyright (c) 2021 Sendbird, Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SBCommonStruct.h"
#include "SBChannelInterface.generated.h"

UINTERFACE(BlueprintType, MinimalAPI)
class USBChannelInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class SENDBIRDSAMPLE_API ISBChannelInterface
{
	GENERATED_IINTERFACE_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ChannelInterface")
	void OnMessageReceived(const FSBMessageInfo& MessageInfo);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ChannelInterface")
	void OnMessageDeleted(int64 MessageID);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ChannelInterface")
	void OnMessageUpdated(const FSBMessageInfo& MessageInfo);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ChannelInterface")
	void OnUserJoined(const FSBUserInfo& UserInfo);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ChannelInterface")
	void OnUserLeft(const FSBUserInfo& UserInfo);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ChannelInterface")
	void OnUserEntered(const FSBUserInfo& UserInfo);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ChannelInterface")
	void OnUserExited(const FSBUserInfo& UserInfo);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ChannelInterface")
	void OnInvitationReceived(const FSBChannelInfo& ChannelInfo, const TArray<FSBUserInfo>& UserInfos);
};
