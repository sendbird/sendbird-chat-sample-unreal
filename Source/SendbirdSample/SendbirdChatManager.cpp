// Copyright (c) 2021 Sendbird, Inc. All rights reserved.

#include "SendbirdChatManager.h"
#include "SendbirdSample.h"
#include "SBCommonStruct.h"
#include "SBChannelInterface.h"
#include "SendbirdSample.h"
#include "GameDelegates.h"

SendbirdChatMananger& SendbirdChatMananger::Get()
{
	static SendbirdChatMananger SBChatMananger;
	return SBChatMananger;
}

SendbirdChatMananger::SendbirdChatMananger()
{
	CurrentUser = nullptr;
	OpenChannelListQuery = nullptr;
	GroupChannelListQuery = nullptr;
	UserListQuery = nullptr;
	CurrentChannel = nullptr;
	ChannelInterface = nullptr;
}

SendbirdChatMananger::~SendbirdChatMananger()
{
	Reset();
}

FString SendbirdChatMananger::GetSdkVersion()
{
#if WITH_SENDBIRD
	return WCHAR_TO_TCHAR(SBDMain::GetSdkVersion().c_str());
#else
	return FString(TEXT(""));
#endif
}

bool SendbirdChatMananger::Initialize(const FString& AppID)
{
#if WITH_SENDBIRD
	SBDMain::Init(TCHAR_TO_WCHAR(*AppID));
	if (!ensureMsgf(SBDMain::IsInitialized(), TEXT("[SendbirdChatMananger::Initialize()] SBDMain::Init() failed!!")))
		return false;
#endif

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, TEXT("SendbirdChatMananger::Initialize()"));
	return true;
}

void SendbirdChatMananger::Reset()
{
#if WITH_SENDBIRD
	SBDMain::RemoveAllChannelHandlers();
	SBDMain::RemoveAllUserEventHandlers();
	SBDMain::RemoveAllConnectionHandlers();
#endif

	CurrentUser = nullptr;
	OpenChannelListQuery = nullptr;
	GroupChannelListQuery = nullptr;
	UserListQuery = nullptr;
	CurrentChannel = nullptr;
	ChannelInterface = nullptr;
	OpenChannels.Empty();
	GroupChannels.Empty();
	HistoryMessages.Empty();
	UserList.Empty();
}

void SendbirdChatMananger::SetCurrentUser(class SBDUser* InUser)
{
	CurrentUser = InUser;
}

void SendbirdChatMananger::SetChannelInterface(class UObject* InChannelInterface)
{ 
#if WITH_SENDBIRD
	SBDMain::RemoveAllChannelHandlers();
	SBDMain::AddChannelHandler(TCHAR_TO_WCHAR(TEXT("SendbirdChatMananger")), this);
#endif
	ChannelInterface = InChannelInterface; 
}

class SBDOpenChannelListQuery* SendbirdChatMananger::CreateOpenChannelListQuery()
{
#if WITH_SENDBIRD
	OpenChannelListQuery = SBDOpenChannel::CreateOpenChannelListQuery();
	if (!ensureMsgf(OpenChannelListQuery, TEXT("[SendbirdChatMananger::CreateOpenChannelListQuery()] CreateOpenChannelListQuery() failed!!")))
		return nullptr;

	OpenChannelListQuery->limit = SendbirdChatMananger::CHANNEL_QUERY_LIST_LIMIT;
	return OpenChannelListQuery;
#else
	return nullptr;
#endif
}

SBDOpenChannel* SendbirdChatMananger::GetSelectedOpenChannel(int Index, const FString& Name)
{
#if WITH_SENDBIRD
	if (!ensureMsgf(OpenChannels.IsValidIndex(Index), TEXT("[SendbirdChatMananger::GetSelectedOpenChannel] Wrong Index(%d)!!"), Index))
		return nullptr;

	if (!ensureMsgf(Name.Compare(WCHAR_TO_TCHAR(OpenChannels[Index]->name.c_str())) == 0,
		TEXT("[SendbirdChatMananger::GetSelectedOpenChannel] Wrong Name(%s) != ChannelName(%s)!!"), *Name, WCHAR_TO_TCHAR(OpenChannels[Index]->name.c_str())))
		return nullptr;

	return OpenChannels[Index];
#else
	return nullptr;
#endif
}

class SBDUserListQuery* SendbirdChatMananger::CreateAllUserListQuery()
{
#if WITH_SENDBIRD
	UserListQuery = SBDMain::CreateAllUserListQuery();
	if (!ensureMsgf(UserListQuery, TEXT("[SendbirdChatMananger::CreateAllUserListQuery()] CreateAllUserListQuery() failed!!")))
		return nullptr;

	UserListQuery->limit = SendbirdChatMananger::USER_QUERY_LIST_LIMIT;
	return UserListQuery;
#else
	return nullptr;
#endif
}

class SBDUserListQuery* SendbirdChatMananger::CreateParticipantListQuery(SBDOpenChannel* OpenChannel)
{
#if WITH_SENDBIRD
	UserListQuery = OpenChannel->CreateParticipantListQuery();
	if (!ensureMsgf(UserListQuery, TEXT("[SendbirdChatMananger::CreateParticipantListQuery()] CreateParticipantListQuery() failed!!")))
		return nullptr;

	UserListQuery->limit = SendbirdChatMananger::USER_QUERY_LIST_LIMIT;
	return UserListQuery;
#else
	return nullptr;
#endif
}

bool SendbirdChatMananger::ContainsUserList(const std::wstring& UserID)
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

class SBDGroupChannelListQuery* SendbirdChatMananger::CreateGroupChannelListQuery()
{
#if WITH_SENDBIRD
	GroupChannelListQuery = SBDGroupChannel::CreateMyGroupChannelListQuery();
	if (!ensureMsgf(GroupChannelListQuery, TEXT("[SendbirdChatMananger::CreateGroupChannelListQuery()] CreateMyGroupChannelListQuery() failed!!")))
		return nullptr;

	GroupChannelListQuery->limit = SendbirdChatMananger::CHANNEL_QUERY_LIST_LIMIT;
	GroupChannelListQuery->include_empty_channel = true;
	GroupChannelListQuery->include_member_list = true;
	return GroupChannelListQuery;
#else
	return nullptr;
#endif
}

SBDGroupChannel* SendbirdChatMananger::GetSelectedGroupChannel(int Index, const FString& Name)
{
#if WITH_SENDBIRD
	if (!ensureMsgf(GroupChannels.IsValidIndex(Index), TEXT("[SendbirdChatMananger::GetSelectedGroupChannel] Wrong Index(%d)!!"), Index))
		return nullptr;

	if (!ensureMsgf(Name.Compare(WCHAR_TO_TCHAR(GroupChannels[Index]->name.c_str())) == 0,
		TEXT("[SendbirdChatMananger::GetSelectedGroupChannel] Wrong Name(%s) != ChannelName(%s)!!"), *Name, WCHAR_TO_TCHAR(GroupChannels[Index]->name.c_str())))
		return nullptr;

	return GroupChannels[Index];
#else
	return nullptr;
#endif
}

SBDBaseMessage* SendbirdChatMananger::GetHistoryMessage(uint64 MessageID)
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

bool SendbirdChatMananger::SetHistoryMessage(uint64 MessageID, class SBDBaseMessage* NewMessage)
{
	if (HistoryMessages.Contains(MessageID))
	{
		HistoryMessages[MessageID] = NewMessage;
		return true;
	}
	return false;
}

const FSBMessageInfo SendbirdChatMananger::AddHistoryMessage(class SBDBaseMessage* Message)
{
#if WITH_SENDBIRD
	if (!ensure(Message))
		return FSBMessageInfo();

	FDateTime MessageTime = SendbirdTimeToDateTime(Message->updated_at != 0 ? Message->updated_at : Message->created_at);
	if (Message->message_type == SBDMessageType::User)
	{
		SBDUserMessage* UserMessage = static_cast<SBDUserMessage*>(Message);
		SendbirdChatMananger::Get().GetHistoryMessages().Add(Message->message_id, Message);
		return FSBMessageInfo(Message->message_id, FSBUserInfo(UserMessage->sender),
			(ESBMessageType)UserMessage->message_type, WCHAR_TO_TCHAR(UserMessage->message.c_str()), MessageTime);
	}
	else if (Message->message_type == SBDMessageType::Admin)
	{
		SBDAdminMessage* AdminMessage = static_cast<SBDAdminMessage*>(Message);
		SendbirdChatMananger::Get().GetHistoryMessages().Add(Message->message_id, Message);
		return FSBMessageInfo(Message->message_id, FSBUserInfo(TEXT("Admin"), TEXT("Admin"), TEXT("")),
			(ESBMessageType)AdminMessage->message_type, WCHAR_TO_TCHAR(AdminMessage->message.c_str()), MessageTime);
	}
	else if (Message->message_type == SBDMessageType::File)
	{
		SBDFileMessage* FileMessage = static_cast<SBDFileMessage*>(Message);
		SendbirdChatMananger::Get().GetHistoryMessages().Add(Message->message_id, Message);
		return FSBMessageInfo(Message->message_id, FSBUserInfo(FileMessage->sender),
			(ESBMessageType)FileMessage->message_type, WCHAR_TO_TCHAR(FileMessage->name.c_str()), MessageTime);
	}

	ensureMsgf(false, TEXT("Unknown MessageType : %d"), (int)Message->message_type);
#endif
	return FSBMessageInfo();
}

bool SendbirdChatMananger::DeleteHistoryMessage(uint64 MessageID)
{
	return (0 < HistoryMessages.Remove(MessageID));
}

const FSBMessageInfo SendbirdChatMananger::UpdateMessage(class SBDBaseMessage* Message)
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

void SendbirdChatMananger::MessageReceived(SBDBaseChannel* channel, SBDBaseMessage* message)
{
#if WITH_SENDBIRD
	if (channel != CurrentChannel)
		return;

	if (!ChannelInterface.IsValid())
		return;

	if (ensure(ChannelInterface->GetClass()->ImplementsInterface(USBChannelInterface::StaticClass())))
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
			if (SendbirdChatMananger::Get().GetChannelInterface().IsValid())
				ISBChannelInterface::Execute_OnMessageReceived(SendbirdChatMananger::Get().GetChannelInterface().Get(), AddedMessage);
		});
	}
#endif
}

void SendbirdChatMananger::MessageUpdated(SBDBaseChannel* channel, SBDBaseMessage* message)
{
#if WITH_SENDBIRD
	if (channel != CurrentChannel)
		return;

	if (!ChannelInterface.IsValid())
		return;

	if (ensure(ChannelInterface->GetClass()->ImplementsInterface(USBChannelInterface::StaticClass())))
	{
		const FSBMessageInfo UpdatedMessage = UpdateMessage(message);
		AsyncTask(ENamedThreads::GameThread, [UpdatedMessage]() {
			if (SendbirdChatMananger::Get().GetChannelInterface().IsValid())
				ISBChannelInterface::Execute_OnMessageUpdated(SendbirdChatMananger::Get().GetChannelInterface().Get(), UpdatedMessage);
		});
	}
#endif
}

void SendbirdChatMananger::MessageDeleted(SBDBaseChannel* channel, uint64_t message_id)
{
#if WITH_SENDBIRD
	if (channel != CurrentChannel)
		return;

	if (!ChannelInterface.IsValid())
		return;

	if (ensure(ChannelInterface->GetClass()->ImplementsInterface(USBChannelInterface::StaticClass())))
	{
		ensure(DeleteHistoryMessage(message_id));
		AsyncTask(ENamedThreads::GameThread, [message_id]() {
			if (SendbirdChatMananger::Get().GetChannelInterface().IsValid())
				ISBChannelInterface::Execute_OnMessageDeleted(SendbirdChatMananger::Get().GetChannelInterface().Get(), message_id);
		});
	}
#endif
}

void SendbirdChatMananger::UserJoined(SBDGroupChannel* channel, SBDUser& user)
{
	if (channel != CurrentChannel)
		return;

	ProcessChannelUserHandler(ESBChannelUserHandlerType::UserJoined, user);
}

void SendbirdChatMananger::UserLeft(SBDGroupChannel* channel, SBDUser& user)
{
	if (channel != CurrentChannel)
		return;

	if (GetCurrentUser() && user.user_id == GetCurrentUser()->user_id)
		ResetCurrentChannel();

	ProcessChannelUserHandler(ESBChannelUserHandlerType::UserLeft, user);
}

void SendbirdChatMananger::UserEntered(SBDOpenChannel* channel, SBDUser& user)
{
	if (channel != CurrentChannel)
		return;
	
	ProcessChannelUserHandler(ESBChannelUserHandlerType::UserEntered, user);
}

void SendbirdChatMananger::UserExited(SBDOpenChannel* channel, SBDUser& user)
{
	if (channel != CurrentChannel)
		return;
	
	ProcessChannelUserHandler(ESBChannelUserHandlerType::UserExited, user);
}

void SendbirdChatMananger::InvitationReceived(SBDGroupChannel* channel, const std::vector<SBDUser>& invitees, SBDUser& inviter)
{
#if WITH_SENDBIRD
	if (!CurrentUser || channel == CurrentChannel)
		return;

	TArray<FSBUserInfo> UserInfos;
	bool bIsInvitee = false;
	for (const SBDUser& invitee : invitees)
	{
		if (CurrentUser->user_id == invitee.user_id)
		{
			bIsInvitee = true;
			continue;
		}
		UserInfos.Add(FSBUserInfo(invitee));
	}

	if (!bIsInvitee)
		return;

	if (!ChannelInterface.IsValid())
		return;

	if (ensure(ChannelInterface->GetClass()->ImplementsInterface(USBChannelInterface::StaticClass())))
	{
		if (!ensure(channel->is_group_channel))
			return;

		AsyncTask(ENamedThreads::GameThread, [UserInfos, channel]()
		{
			//if (SendbirdChatMananger::Get().GetChannelInterface().IsValid())
			//{
			//	SendbirdChatMananger::Get().SetCurrentChannel(channel);
			//	ISBChannelInterface::Execute_OnInvitationReceived(SendbirdChatMananger::Get().GetChannelInterface().Get(), FSBChannelInfo(channel), UserInfos);
			//}
		});
	}
#endif
}

void SendbirdChatMananger::ProcessChannelUserHandler(ESBChannelUserHandlerType HandlerType, SBDUser& user)
{
#if WITH_SENDBIRD
	if (!ChannelInterface.IsValid())
		return;

	if (ensure(ChannelInterface->GetClass()->ImplementsInterface(USBChannelInterface::StaticClass())))
	{
		FSBUserInfo UserInfo(user);
		AsyncTask(ENamedThreads::GameThread, [HandlerType, UserInfo]()
		{
			FString Message;
			switch (HandlerType)
			{
			case ESBChannelUserHandlerType::UserEntered:
				if (UserInfo.UserID != WCHAR_TO_TCHAR(SendbirdChatMananger::Get().GetCurrentUser()->user_id.c_str()))
					Message = FString::Printf(TEXT("%s Entered."), *UserInfo.NickName);
				if (SendbirdChatMananger::Get().GetChannelInterface().IsValid())
					ISBChannelInterface::Execute_OnUserEntered(SendbirdChatMananger::Get().GetChannelInterface().Get(), UserInfo);
				break;

			case ESBChannelUserHandlerType::UserExited:
				if (UserInfo.UserID != WCHAR_TO_TCHAR(SendbirdChatMananger::Get().GetCurrentUser()->user_id.c_str()))
					Message = FString::Printf(TEXT("%s Exited."), *UserInfo.NickName);
				if (SendbirdChatMananger::Get().GetChannelInterface().IsValid())
					ISBChannelInterface::Execute_OnUserExited(SendbirdChatMananger::Get().GetChannelInterface().Get(), UserInfo);
				break;

			case ESBChannelUserHandlerType::UserJoined:
				if (SendbirdChatMananger::Get().GetChannelInterface().IsValid())
					ISBChannelInterface::Execute_OnUserJoined(SendbirdChatMananger::Get().GetChannelInterface().Get(), UserInfo);
				break;

			case ESBChannelUserHandlerType::UserLeft:
				if (SendbirdChatMananger::Get().GetChannelInterface().IsValid())
					ISBChannelInterface::Execute_OnUserLeft(SendbirdChatMananger::Get().GetChannelInterface().Get(), UserInfo);
				break;
			}

			if (!Message.IsEmpty())
			{
				const FSBMessageInfo AddedMessage(-1, UserInfo, ESBMessageType::SBDMessageTypeAdmin, Message, FDateTime::Now());
				if (SendbirdChatMananger::Get().GetChannelInterface().IsValid())
					ISBChannelInterface::Execute_OnMessageReceived(SendbirdChatMananger::Get().GetChannelInterface().Get(), AddedMessage);
			}
		});
	}
#endif
}

FDateTime SendbirdChatMananger::SendbirdTimeToDateTime(int64 Time)
{
	return FDateTime(1970, 1, 1) + FTimespan(Time * ETimespan::TicksPerMillisecond);
}
