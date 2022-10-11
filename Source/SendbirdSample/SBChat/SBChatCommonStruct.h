// Copyright (c) 2021 Sendbird, Inc. All rights reserved.

#pragma once

#include "Sendbird/include/Sendbird.h"
#include "SBChatCommonEnum.h"
#include "SBChatCommonStruct.generated.h"

USTRUCT(BlueprintType)
struct FSBUserInfo
{
	GENERATED_USTRUCT_BODY()

	FSBUserInfo() : UserID(TEXT("")), NickName(TEXT("")), ProfileUrl(TEXT("")) {}
	FSBUserInfo(const FString& InUserID, const FString& InNickName, const FString& InProfileUrl)
		: UserID(InUserID), NickName(InNickName), ProfileUrl(InProfileUrl) {}
	FSBUserInfo(SBDUser* User) : FSBUserInfo(WCHAR_TO_TCHAR(User->user_id.c_str()), WCHAR_TO_TCHAR(User->nickname.c_str()), WCHAR_TO_TCHAR(User->profile_url.c_str())) {}
	FSBUserInfo(SBDUser User) : FSBUserInfo(&User) {}

	void Init()
	{
		UserID.Empty();
		NickName.Empty();
		ProfileUrl.Empty();
	}

	UPROPERTY(BlueprintReadWrite)
	FString UserID;
	UPROPERTY(BlueprintReadWrite)
	FString NickName;
	UPROPERTY(BlueprintReadWrite)
	FString ProfileUrl;
};

USTRUCT(BlueprintType)
struct FSBChannelInfo
{
	GENERATED_USTRUCT_BODY()

	FSBChannelInfo()
		: Name(TEXT("")), IsGroupChannel(false), MemberCount(0), UnreadMessageCount(0) {}
	FSBChannelInfo(const FString& InName, bool InIsGroupChannel = false, int InMemberCount = 1, int InUnreadMessageCount = 0)
		: Name(InName), IsGroupChannel(InIsGroupChannel), MemberCount(InMemberCount), UnreadMessageCount(InUnreadMessageCount) {}
	FSBChannelInfo(SBDBaseChannel* Channel) : FSBChannelInfo(WCHAR_TO_TCHAR(Channel->name.c_str()), Channel->is_group_channel)
	{
		if (IsGroupChannel)
		{
			SBDGroupChannel* GroupChannel = static_cast<SBDGroupChannel*>(Channel);
			MemberCount = GroupChannel->member_count;
			UnreadMessageCount = GroupChannel->unread_message_count;
		}
		else
		{
			SBDOpenChannel* OpenChannel = static_cast<SBDOpenChannel*>(Channel);
			MemberCount = OpenChannel->participant_count;
		}
	}

	UPROPERTY(BlueprintReadWrite)
	FString Name;
	UPROPERTY(BlueprintReadWrite)
	bool IsGroupChannel;
	UPROPERTY(BlueprintReadWrite)
	int MemberCount;						
	UPROPERTY(BlueprintReadWrite)
	int UnreadMessageCount;
}; 

USTRUCT(BlueprintType)
struct FSBMessageInfo
{
	GENERATED_USTRUCT_BODY()

	FSBMessageInfo()
		: MessageID(-1), UserInfo(FSBUserInfo()),MessageType(ESBMessageType::SBDMessageTypeUser), Message(TEXT("")), UpdatedTime(FDateTime::Now()) {}
	FSBMessageInfo(uint64 InMessageID, const FSBUserInfo& InUserInfo, ESBMessageType InMessageType, const FString& InMessage, const FDateTime& InUpdatedTime)
		: MessageID(InMessageID), UserInfo(InUserInfo), MessageType(InMessageType), Message(InMessage), UpdatedTime(InUpdatedTime) {}

	UPROPERTY(BlueprintReadWrite)
	int64 MessageID;
	UPROPERTY(BlueprintReadWrite)
	FSBUserInfo UserInfo;
	UPROPERTY(BlueprintReadWrite)
	ESBMessageType MessageType;
	UPROPERTY(BlueprintReadWrite)
	FString Message;
	UPROPERTY(BlueprintReadWrite)
	FDateTime UpdatedTime;
};
