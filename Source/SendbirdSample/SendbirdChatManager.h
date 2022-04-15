// Copyright (c) 2021 Sendbird, Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Sendbird/include/Sendbird.h"
#include "SBCommonEnum.h"

class SendbirdChatMananger : public SBDChannelHandler
{
public:
	static const int CHANNEL_QUERY_LIST_LIMIT	= 20;
	static const int USER_QUERY_LIST_LIMIT		= 10;
	static const int MESSAGE_QUERY_LIST_LIMIT	= 15;

	static SendbirdChatMananger& Get();
	SendbirdChatMananger();
	virtual ~SendbirdChatMananger();

	FString							GetSdkVersion();
	bool							Initialize(const FString& AppID);
	void							Reset();
	void							SetCurrentUser(class SBDUser* InUser);
	class SBDUser*					GetCurrentUser() { return CurrentUser; }
	void							SetChannelInterface(class UObject* InChannelInterface);
	TWeakObjectPtr<class UObject>	GetChannelInterface() { return ChannelInterface; }
	const FString&					GetPushTokenString() const { return PushTokenString; }

	//+ OpenChannel
	class SBDOpenChannelListQuery*	CreateOpenChannelListQuery();
	class SBDOpenChannelListQuery*	GetOpenChannelListQuery() { return OpenChannelListQuery; }
	TArray<class SBDOpenChannel*>&	GetOpenChannels() { return OpenChannels; }
	void							SetOpenChannels(const TArray<class SBDOpenChannel*>& InOpenChannels) { OpenChannels = InOpenChannels; }
	void							ResetOpenChannels() { OpenChannels.Empty(); ResetCurrentChannel(); }
	class SBDOpenChannel*			GetSelectedOpenChannel(int Index, const FString& Name);
	//- OpenChannel
	
	//+ Friend 
	class SBDUserListQuery*			CreateAllUserListQuery();
	class SBDUserListQuery*			CreateParticipantListQuery(SBDOpenChannel* OpenChannel);
	class SBDUserListQuery*			GetUserListQuery() { return UserListQuery; }
	TArray<class SBDUser>&			GetUserList() { return UserList; }
	void							ResetUserList() { UserList.Empty(); }
	bool							ContainsUserList(const std::wstring& UserID);
	//- Friend 

	//+ GroupChannel
	class SBDGroupChannelListQuery*	CreateGroupChannelListQuery();
	class SBDGroupChannelListQuery*	GetGroupChannelListQuery() { return GroupChannelListQuery; }
	TArray<class SBDGroupChannel*>&	GetGroupChannels() { return GroupChannels; }
	void							SetGroupChannels(const TArray<class SBDGroupChannel*>& InGroupChannels) { GroupChannels = InGroupChannels; }
	void							ResetGroupChannels() { GroupChannels.Empty(); ResetCurrentChannel(); }
	class SBDGroupChannel*			GetSelectedGroupChannel(int Index, const FString& Name);
	//- GroupChannel

	class SBDBaseChannel*			GetCurrentChannel() const { return CurrentChannel; }
	void							SetCurrentChannel(class SBDBaseChannel* Channel) { CurrentChannel = Channel; }
	void							ResetCurrentChannel() { CurrentChannel = nullptr; }
	TMap<int64,class SBDBaseMessage*>&	GetHistoryMessages() { return HistoryMessages; }
	void							ResetHistoryMessage() { HistoryMessages.Empty(); }
	class SBDBaseMessage*			GetHistoryMessage(uint64 MessageID);
	bool							SetHistoryMessage(uint64 MessageID, class SBDBaseMessage* NewMessage);
	const struct FSBMessageInfo		AddHistoryMessage(class SBDBaseMessage* Message);
	bool							DeleteHistoryMessage(uint64 MessageID);
	const struct FSBMessageInfo		UpdateMessage(class SBDBaseMessage* Message);
	
	//+ SBDChannelHandler 
	virtual void					MessageReceived(SBDBaseChannel* channel, SBDBaseMessage* message) override;
	virtual void					MessageUpdated(SBDBaseChannel* channel, SBDBaseMessage* message) override;
	virtual void					MessageDeleted(SBDBaseChannel* channel, uint64_t message_id) override;
	virtual void					UserJoined(SBDGroupChannel* channel, SBDUser& user) override;
	virtual void					UserLeft(SBDGroupChannel* channel, SBDUser& user) override;
	virtual void					UserEntered(SBDOpenChannel* channel, SBDUser& user) override;
	virtual void					UserExited(SBDOpenChannel* channel, SBDUser& user) override;
	virtual void					InvitationReceived(SBDGroupChannel* channel, const std::vector<SBDUser>& invitees, SBDUser& inviter) override;
	//- SBDChannelHandler 

private:
	void							ProcessChannelUserHandler(ESBChannelUserHandlerType HandlerType, SBDUser& user);
	FDateTime						SendbirdTimeToDateTime(int64 Time);

private:
	class SBDUser*					CurrentUser;
	
	//+ OpenChannel
	class SBDOpenChannelListQuery*	OpenChannelListQuery;
	TArray<class SBDOpenChannel*>	OpenChannels;
	//- OpenChannel

	//+ OpenChannel
	class SBDGroupChannelListQuery*	GroupChannelListQuery;
	TArray<class SBDGroupChannel*>	GroupChannels;
	//- OpenChannel

	//+ UserList
	class SBDUserListQuery*			UserListQuery;
	TArray<class SBDUser>			UserList;
	//- UserList
	
	class SBDBaseChannel*			CurrentChannel;
	TMap<int64, class SBDBaseMessage*>	HistoryMessages;

	TWeakObjectPtr<class UObject>	ChannelInterface;
	FString							PushTokenString;
};
