// Copyright (c) 2021 Sendbird, Inc. All rights reserved.

#include "SBChatManager.h"
#include "SBChatCommonStruct.h"
#include "SBChatChannelEvent.h"

SBChatManager& SBChatManager::Get()
{
	static SBChatManager SBChatMananger;
	return SBChatMananger;
}

SBChatManager::SBChatManager()
{
	ChannelEvent = nullptr;
	CurrentChannel = nullptr;

	OpenChannelListQuery = nullptr;
	GroupChannelListQuery = nullptr;
	UserListQuery = nullptr;
}

SBChatManager::~SBChatManager()
{
	Reset();
}

//+ Common
void SBChatManager::Reset()
{
#if WITH_SENDBIRD
	SBDMain::RemoveAllChannelHandlers();
	SBDMain::RemoveAllUserEventHandlers();
	SBDMain::RemoveAllConnectionHandlers();
#endif

	ChannelEvent = nullptr;
	CurrentChannel = nullptr;
	HistoryMessages.Empty();

	CachedProfileTexture.Reset();

	OpenChannelListQuery = nullptr;
	OpenChannels.Empty();

	GroupChannelListQuery = nullptr;
	GroupChannels.Empty();
	
	UserListQuery = nullptr;
	UserList.Empty();
}

void SBChatManager::SetChannelEvent(UObject* InChannelEvent)
{ 
#if WITH_SENDBIRD
	SBDMain::RemoveAllChannelHandlers();
	SBDMain::AddChannelHandler(TCHAR_TO_WCHAR(TEXT("SBChatManager")), this);
#endif
	ChannelEvent = InChannelEvent; 
}

SBDBaseMessage* SBChatManager::GetHistoryMessage(uint64 MessageID)
{
#if WITH_SENDBIRD
	SBDBaseMessage** Message = HistoryMessages.Find(MessageID);
	if (nullptr == Message)
		return nullptr;

	return *Message;
#else
	return nullptr;
#endif
}

bool SBChatManager::SetHistoryMessage(uint64 MessageID, SBDBaseMessage* NewMessage)
{
	if (HistoryMessages.Contains(MessageID))
	{
		HistoryMessages[MessageID] = NewMessage;
		return true;
	}
	return false;
}

const FSBMessageInfo SBChatManager::AddHistoryMessage(SBDBaseMessage* Message)
{
#if WITH_SENDBIRD
	if (!ensure(Message))
		return FSBMessageInfo();

	FDateTime MessageTime = SendbirdTimeToDateTime(Message->updated_at != 0 ? Message->updated_at : Message->created_at);
	if (Message->message_type == SBDMessageType::User)
	{
		SBDUserMessage* UserMessage = static_cast<SBDUserMessage*>(Message);
		SBChatManager::Get().GetHistoryMessages().Add(Message->message_id, Message);
		return FSBMessageInfo(Message->message_id, FSBUserInfo(UserMessage->sender),
			(ESBMessageType)UserMessage->message_type, WCHAR_TO_TCHAR(UserMessage->message.c_str()), MessageTime);
	}
	else if (Message->message_type == SBDMessageType::Admin)
	{
		SBDAdminMessage* AdminMessage = static_cast<SBDAdminMessage*>(Message);
		SBChatManager::Get().GetHistoryMessages().Add(Message->message_id, Message);
		return FSBMessageInfo(Message->message_id, FSBUserInfo(TEXT("Admin"), TEXT("Admin"), TEXT("")),
			(ESBMessageType)AdminMessage->message_type, WCHAR_TO_TCHAR(AdminMessage->message.c_str()), MessageTime);
	}
	else if (Message->message_type == SBDMessageType::File)
	{
		SBDFileMessage* FileMessage = static_cast<SBDFileMessage*>(Message);
		SBChatManager::Get().GetHistoryMessages().Add(Message->message_id, Message);
		return FSBMessageInfo(Message->message_id, FSBUserInfo(FileMessage->sender),
			(ESBMessageType)FileMessage->message_type, WCHAR_TO_TCHAR(FileMessage->name.c_str()), MessageTime);
	}

	ensureMsgf(false, TEXT("Unknown MessageType : %d"), (int)Message->message_type);
#endif
	return FSBMessageInfo();
}

bool SBChatManager::DeleteHistoryMessage(uint64 MessageID)
{
	return (0 < HistoryMessages.Remove(MessageID));
}

const FSBMessageInfo SBChatManager::UpdateHistoryMessage(SBDBaseMessage* Message)
{
#if WITH_SENDBIRD
	if (!ensure(Message))
		return FSBMessageInfo();

	bool bUpdated = SetHistoryMessage(Message->message_id, Message);
	if (!ensure(bUpdated))
		return FSBMessageInfo();

	FDateTime MessageTime = SendbirdTimeToDateTime(Message->updated_at != 0 ? Message->updated_at : Message->created_at);
	if (Message->message_type == SBDMessageType::User)
	{
		SBDUserMessage* UserMessage = static_cast<SBDUserMessage*>(Message);
		return FSBMessageInfo(Message->message_id, FSBUserInfo(UserMessage->sender),
			(ESBMessageType)UserMessage->message_type, WCHAR_TO_TCHAR(UserMessage->message.c_str()), MessageTime);
	}
	else if (Message->message_type == SBDMessageType::Admin)
	{
		SBDAdminMessage* AdminMessage = static_cast<SBDAdminMessage*>(Message);
		return FSBMessageInfo(Message->message_id, FSBUserInfo(TEXT("Admin"), TEXT("Admin"), TEXT("")),
			(ESBMessageType)AdminMessage->message_type, WCHAR_TO_TCHAR(AdminMessage->message.c_str()), MessageTime);
	}
	else if (Message->message_type == SBDMessageType::File)
	{
		SBDFileMessage* FileMessage = static_cast<SBDFileMessage*>(Message);
		return FSBMessageInfo(Message->message_id, FSBUserInfo(FileMessage->sender),
			(ESBMessageType)FileMessage->message_type, WCHAR_TO_TCHAR(FileMessage->name.c_str()), MessageTime);
	}

	ensureMsgf(false, TEXT("Unknown MessageType : %d"), (int)Message->message_type);
#endif
	return FSBMessageInfo();
}
//- Common

//+ User
SBDUserListQuery* SBChatManager::CreateAllUserListQuery()
{
#if WITH_SENDBIRD
	UserListQuery = SBDMain::CreateAllUserListQuery();
	if (!ensureMsgf(UserListQuery, TEXT("[SBChatManager::CreateAllUserListQuery()] CreateAllUserListQuery() failed!!")))
		return nullptr;

	UserListQuery->limit = SBChatManager::USER_QUERY_LIST_LIMIT;
	return UserListQuery;
#else
	return nullptr;
#endif
}

bool SBChatManager::IsInUserList(const std::wstring& UserID)
{
#if WITH_SENDBIRD
	for (const SBDUser& User : UserList)
	{
		if (User.user_id == UserID)
			return true;
	}
#endif
	return false;
}
//- User

//+ OpenChannel
SBDOpenChannelListQuery* SBChatManager::CreateOpenChannelListQuery()
{
#if WITH_SENDBIRD
	OpenChannelListQuery = SBDOpenChannel::CreateOpenChannelListQuery();
	if (!ensureMsgf(OpenChannelListQuery, TEXT("[SBChatManager::CreateOpenChannelListQuery()] CreateOpenChannelListQuery() failed!!")))
		return nullptr;

	OpenChannelListQuery->limit = SBChatManager::CHANNEL_QUERY_LIST_LIMIT;
	return OpenChannelListQuery;
#else
	return nullptr;
#endif
}

SBDOpenChannel* SBChatManager::GetSelectedOpenChannel(int Index, const FString& Name)
{
#if WITH_SENDBIRD
	if (!ensureMsgf(OpenChannels.IsValidIndex(Index), TEXT("[SBChatManager::GetSelectedOpenChannel] Wrong Index(%d)!!"), Index))
		return nullptr;

	if (!ensureMsgf(Name.Compare(WCHAR_TO_TCHAR(OpenChannels[Index]->name.c_str())) == 0,
		TEXT("[SBChatManager::GetSelectedOpenChannel] Wrong Name(%s) != ChannelName(%s)!!"), *Name, WCHAR_TO_TCHAR(OpenChannels[Index]->name.c_str())))
		return nullptr;

	return OpenChannels[Index];
#else
	return nullptr;
#endif
}

SBDUserListQuery* SBChatManager::CreateParticipantListQuery(SBDOpenChannel* OpenChannel)
{
#if WITH_SENDBIRD
	UserListQuery = OpenChannel->CreateParticipantListQuery();
	if (!ensureMsgf(UserListQuery, TEXT("[SBChatManager::CreateParticipantListQuery()] CreateParticipantListQuery() failed!!")))
		return nullptr;

	UserListQuery->limit = SBChatManager::USER_QUERY_LIST_LIMIT;
	return UserListQuery;
#else
	return nullptr;
#endif
}
//- OpenChannel

//+ GroupChannel
SBDGroupChannelListQuery* SBChatManager::CreateGroupChannelListQuery()
{
#if WITH_SENDBIRD
	GroupChannelListQuery = SBDGroupChannel::CreateMyGroupChannelListQuery();
	if (!ensureMsgf(GroupChannelListQuery, TEXT("[SBChatManager::CreateGroupChannelListQuery()] CreateMyGroupChannelListQuery() failed!!")))
		return nullptr;

	GroupChannelListQuery->limit = SBChatManager::CHANNEL_QUERY_LIST_LIMIT;
	GroupChannelListQuery->include_empty_channel = true;
	GroupChannelListQuery->include_member_list = true;
	return GroupChannelListQuery;
#else
	return nullptr;
#endif
}

SBDGroupChannel* SBChatManager::GetSelectedGroupChannel(int Index, const FString& Name)
{
#if WITH_SENDBIRD
	if (!ensureMsgf(GroupChannels.IsValidIndex(Index), TEXT("[SBChatManager::GetSelectedGroupChannel] Wrong Index(%d)!!"), Index))
		return nullptr;

	if (!ensureMsgf(Name.Compare(WCHAR_TO_TCHAR(GroupChannels[Index]->name.c_str())) == 0,
		TEXT("[SBChatManager::GetSelectedGroupChannel] Wrong Name(%s) != ChannelName(%s)!!"), *Name, WCHAR_TO_TCHAR(GroupChannels[Index]->name.c_str())))
		return nullptr;

	return GroupChannels[Index];
#else
	return nullptr;
#endif
}
//- GroupChannel

//+ SBDChannelHandler
void SBChatManager::MessageReceived(SBDBaseChannel* channel, SBDBaseMessage* message)
{
#if WITH_SENDBIRD
	if (channel != CurrentChannel)
		return;

	if (!ChannelEvent.IsValid())
		return;

	if (ensure(ChannelEvent->GetClass()->ImplementsInterface(USBChatChannelEvent::StaticClass())))
	{
		if (channel->is_group_channel)
		{
			SBDGroupChannel* GroupChannel = static_cast<SBDGroupChannel*>(channel);
			GroupChannel->MarkAsRead();
		}

		if (GetHistoryMessage(message->message_id) != nullptr)
			return;

		const FSBMessageInfo AddedMessage = AddHistoryMessage(message);
		AsyncTask(ENamedThreads::GameThread, [AddedMessage]() {
			if (SBChatManager::Get().GetChannelEvent().IsValid())
				ISBChatChannelEvent::Execute_OnMessageReceived(SBChatManager::Get().GetChannelEvent().Get(), AddedMessage);
		});
	}
#endif
}

void SBChatManager::MessageUpdated(SBDBaseChannel* channel, SBDBaseMessage* message)
{
#if WITH_SENDBIRD
	if (channel != CurrentChannel)
		return;

	if (!ChannelEvent.IsValid())
		return;

	if (ensure(ChannelEvent->GetClass()->ImplementsInterface(USBChatChannelEvent::StaticClass())))
	{
		const FSBMessageInfo UpdatedMessage = UpdateHistoryMessage(message);
		AsyncTask(ENamedThreads::GameThread, [UpdatedMessage]() {
			if (SBChatManager::Get().GetChannelEvent().IsValid())
				ISBChatChannelEvent::Execute_OnMessageUpdated(SBChatManager::Get().GetChannelEvent().Get(), UpdatedMessage);
		});
	}
#endif
}

void SBChatManager::MessageDeleted(SBDBaseChannel* channel, uint64_t message_id)
{
#if WITH_SENDBIRD
	if (channel != CurrentChannel)
		return;

	if (!ChannelEvent.IsValid())
		return;

	if (ensure(ChannelEvent->GetClass()->ImplementsInterface(USBChatChannelEvent::StaticClass())))
	{
		ensure(DeleteHistoryMessage(message_id));
		AsyncTask(ENamedThreads::GameThread, [message_id]() {
			if (SBChatManager::Get().GetChannelEvent().IsValid())
				ISBChatChannelEvent::Execute_OnMessageDeleted(SBChatManager::Get().GetChannelEvent().Get(), message_id);
		});
	}
#endif
}

void SBChatManager::UserJoined(SBDGroupChannel* channel, SBDUser& user)
{
	if (channel != CurrentChannel)
		return;

	ProcessChannelUserHandler(ESBChannelUserHandlerType::UserJoined, user);
}

void SBChatManager::UserLeft(SBDGroupChannel* channel, SBDUser& user)
{
	if (channel != CurrentChannel)
		return;

	if (SBDMain::GetCurrentUser() && user.user_id == SBDMain::GetCurrentUser()->user_id)
		ResetCurrentChannel();

	ProcessChannelUserHandler(ESBChannelUserHandlerType::UserLeft, user);
}

void SBChatManager::UserEntered(SBDOpenChannel* channel, SBDUser& user)
{
	if (channel != CurrentChannel)
		return;
	
	ProcessChannelUserHandler(ESBChannelUserHandlerType::UserEntered, user);
}

void SBChatManager::UserExited(SBDOpenChannel* channel, SBDUser& user)
{
	if (channel != CurrentChannel)
		return;
	
	ProcessChannelUserHandler(ESBChannelUserHandlerType::UserExited, user);
}

void SBChatManager::InvitationReceived(SBDGroupChannel* channel, const std::vector<SBDUser>& invitees, SBDUser& inviter)
{
#if WITH_SENDBIRD
	if (SBDMain::GetCurrentUser() == nullptr || channel == CurrentChannel)
		return;

	TArray<FSBUserInfo> UserInfos;
	bool bIsInvitee = false;
	for (const SBDUser& invitee : invitees)
	{
		if (SBDMain::GetCurrentUser()->user_id == invitee.user_id)
		{
			bIsInvitee = true;
			continue;
		}
		UserInfos.Add(FSBUserInfo(invitee));
	}

	if (!bIsInvitee)
		return;

	if (!ChannelEvent.IsValid())
		return;

	if (ensure(ChannelEvent->GetClass()->ImplementsInterface(USBChatChannelEvent::StaticClass())))
	{
		if (!ensure(channel->is_group_channel))
			return;

		AsyncTask(ENamedThreads::GameThread, [UserInfos, channel]()
		{
			if (SBChatManager::Get().GetChannelEvent().IsValid())
			{
				SBChatManager::Get().SetCurrentChannel(channel);
				ISBChatChannelEvent::Execute_OnInvitationReceived(SBChatManager::Get().GetChannelEvent().Get(), FSBChannelInfo(channel), UserInfos);
			}
		});
	}
#endif
}
//- SBDChannelHandler

//+ private
void SBChatManager::ProcessChannelUserHandler(ESBChannelUserHandlerType HandlerType, SBDUser& user)
{
#if WITH_SENDBIRD
	if (!ChannelEvent.IsValid())
		return;

	if (ensure(ChannelEvent->GetClass()->ImplementsInterface(USBChatChannelEvent::StaticClass())))
	{
		FSBUserInfo UserInfo(user);
		AsyncTask(ENamedThreads::GameThread, [HandlerType, UserInfo]()
		{
			FString Message;
			switch (HandlerType)
			{
			case ESBChannelUserHandlerType::UserEntered:
				if (SBDMain::GetCurrentUser() != nullptr && UserInfo.UserID != WCHAR_TO_TCHAR(SBDMain::GetCurrentUser()->user_id.c_str()))
					Message = FString::Printf(TEXT("%s Entered."), *UserInfo.NickName);
				if (SBChatManager::Get().GetChannelEvent().IsValid())
					ISBChatChannelEvent::Execute_OnUserEntered(SBChatManager::Get().GetChannelEvent().Get(), UserInfo);
				break;

			case ESBChannelUserHandlerType::UserExited:
				if (SBDMain::GetCurrentUser() != nullptr && UserInfo.UserID != WCHAR_TO_TCHAR(SBDMain::GetCurrentUser()->user_id.c_str()))
					Message = FString::Printf(TEXT("%s Exited."), *UserInfo.NickName);
				if (SBChatManager::Get().GetChannelEvent().IsValid())
					ISBChatChannelEvent::Execute_OnUserExited(SBChatManager::Get().GetChannelEvent().Get(), UserInfo);
				break;

			case ESBChannelUserHandlerType::UserJoined:
				if (SBChatManager::Get().GetChannelEvent().IsValid())
					ISBChatChannelEvent::Execute_OnUserJoined(SBChatManager::Get().GetChannelEvent().Get(), UserInfo);
				break;

			case ESBChannelUserHandlerType::UserLeft:
				if (SBChatManager::Get().GetChannelEvent().IsValid())
					ISBChatChannelEvent::Execute_OnUserLeft(SBChatManager::Get().GetChannelEvent().Get(), UserInfo);
				break;
			}

			if (!Message.IsEmpty())
			{
				const FSBMessageInfo AddedMessage(-1, UserInfo, ESBMessageType::SBDMessageTypeAdmin, Message, FDateTime::Now());
				if (SBChatManager::Get().GetChannelEvent().IsValid())
					ISBChatChannelEvent::Execute_OnMessageReceived(SBChatManager::Get().GetChannelEvent().Get(), AddedMessage);
			}
		});
	}
#endif
}

FDateTime SBChatManager::SendbirdTimeToDateTime(int64 Time)
{
	return FDateTime(1970, 1, 1) + FTimespan(Time * ETimespan::TicksPerMillisecond);
}
//- private
