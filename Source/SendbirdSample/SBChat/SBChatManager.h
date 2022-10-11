// Copyright (c) 2021 Sendbird, Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Sendbird/include/Sendbird.h"
#include "SBChatCommonEnum.h"

class SBChatManager : public SBDChannelHandler
{	
private:
	SBChatManager();

public:
	static const int CHANNEL_QUERY_LIST_LIMIT	= 20;
	static const int USER_QUERY_LIST_LIMIT		= 10;
	static const int MESSAGE_QUERY_LIST_LIMIT	= 15;

	static SBChatManager& Get();
	virtual ~SBChatManager();

	//+ Common
	void								Reset();
	void								SetChannelEvent(UObject* InChannelEvent);
	TWeakObjectPtr<UObject>				GetChannelEvent() { return ChannelEvent; }

	SBDBaseChannel*						GetCurrentChannel() const { return CurrentChannel; }
	void								SetCurrentChannel(SBDBaseChannel* Channel) { CurrentChannel = Channel; }
	void								ResetCurrentChannel() { CurrentChannel = nullptr; }

	TMap<int64, SBDBaseMessage*>&		GetHistoryMessages() { return HistoryMessages; }
	void								ResetHistoryMessage() { HistoryMessages.Empty(); }
	SBDBaseMessage*						GetHistoryMessage(uint64 MessageID);
	bool								SetHistoryMessage(uint64 MessageID, SBDBaseMessage* NewMessage);
	const struct FSBMessageInfo			AddHistoryMessage(SBDBaseMessage* Message);
	bool								DeleteHistoryMessage(uint64 MessageID);
	const struct FSBMessageInfo			UpdateHistoryMessage(SBDBaseMessage* Message);
	//- Common

	//+ User
	TMap<FString, UTexture2DDynamic*>	GetCachedProfileTexture() { return CachedProfileTexture; }
	
	SBDUserListQuery*					GetUserListQuery() { return UserListQuery; }
	TArray<SBDUser>&					GetUserList() { return UserList; }
	void								ResetUserList() { UserList.Empty(); }
	SBDUserListQuery*					CreateAllUserListQuery();
	bool								IsInUserList(const std::wstring& UserID);
	//- User

	//+ OpenChannel
	SBDOpenChannelListQuery*			CreateOpenChannelListQuery();
	SBDOpenChannelListQuery*			GetOpenChannelListQuery() { return OpenChannelListQuery; }
	TArray<SBDOpenChannel*>&			GetOpenChannels() { return OpenChannels; }
	void								SetOpenChannels(const TArray<SBDOpenChannel*>& InOpenChannels) { OpenChannels = InOpenChannels; }
	void								ResetOpenChannels() { OpenChannels.Empty(); ResetCurrentChannel(); }
	SBDOpenChannel*						GetSelectedOpenChannel(int Index, const FString& Name);
	SBDUserListQuery*					CreateParticipantListQuery(SBDOpenChannel* OpenChannel);
	//- OpenChannel

	//+ GroupChannel
	SBDGroupChannelListQuery*			CreateGroupChannelListQuery();
	SBDGroupChannelListQuery*			GetGroupChannelListQuery() { return GroupChannelListQuery; }
	TArray<SBDGroupChannel*>&			GetGroupChannels() { return GroupChannels; }
	void								SetGroupChannels(const TArray<SBDGroupChannel*>& InGroupChannels) { GroupChannels = InGroupChannels; }
	void								ResetGroupChannels() { GroupChannels.Empty(); ResetCurrentChannel(); }
	SBDGroupChannel*					GetSelectedGroupChannel(int Index, const FString& Name);
	//- GroupChannel
	
	//+ SBDChannelHandler
	virtual void						MessageReceived(SBDBaseChannel* channel, SBDBaseMessage* message) override;
	virtual void						MessageUpdated(SBDBaseChannel* channel, SBDBaseMessage* message) override;
	virtual void						MessageDeleted(SBDBaseChannel* channel, uint64_t message_id) override;
	virtual void						UserJoined(SBDGroupChannel* channel, SBDUser& user) override;
	virtual void						UserLeft(SBDGroupChannel* channel, SBDUser& user) override;
	virtual void						UserEntered(SBDOpenChannel* channel, SBDUser& user) override;
	virtual void						UserExited(SBDOpenChannel* channel, SBDUser& user) override;
	virtual void						InvitationReceived(SBDGroupChannel* channel, const std::vector<SBDUser>& invitees, SBDUser& inviter) override;
	//- SBDChannelHandler

private:
	void								ProcessChannelUserHandler(ESBChannelUserHandlerType HandlerType, SBDUser& user);
	FDateTime							SendbirdTimeToDateTime(int64 Time);

private:
	//+ Common
	TWeakObjectPtr<UObject>				ChannelEvent;
	SBDBaseChannel*						CurrentChannel;
	TMap<int64, SBDBaseMessage*>		HistoryMessages;
	//- Common
	
	//+ User
	TMap<FString, UTexture2DDynamic*>	CachedProfileTexture;
	SBDUserListQuery*					UserListQuery;
	TArray<SBDUser>						UserList;
	//- USer

	//+ OpenChannel
	SBDOpenChannelListQuery*			OpenChannelListQuery;
	TArray<SBDOpenChannel*>				OpenChannels;
	//- OpenChannel

	//+ GroupChannel
	SBDGroupChannelListQuery*			GroupChannelListQuery;
	TArray<SBDGroupChannel*>			GroupChannels;
	//- GroupChannel
};
