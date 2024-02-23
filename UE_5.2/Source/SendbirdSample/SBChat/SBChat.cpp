// Copyright (c) 2021 Sendbird, Inc. All rights reserved.

#include "SBChat.h"
#include "Kismet/GameplayStatics.h"
#include "../SendbirdSample.h"
#include "SBChatManager.h"
#include "SBChatChannelEvent.h"

USBChat::USBChat(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

//+ Common
FString USBChat::GetSdkVersion() {
#if WITH_SENDBIRD
	return WCHAR_TO_TCHAR(SBDMain::GetSdkVersion().c_str());
#else
	return FString(TEXT(""));
#endif
}

bool USBChat::Init(const FString& AppID)
{
#if WITH_SENDBIRD
	SBDMain::Init(TCHAR_TO_WCHAR(*AppID));
	if (!ensureMsgf(SBDMain::IsInitialized(), TEXT("[SBChatManager::Init()] SBDMain::Init() failed!!")))
		return false;
#endif
	return true;
}

void USBChat::SetChannelEvent(UObject* InChannelEvent)
{
	if (!ensure(InChannelEvent))
        return;

    if (InChannelEvent->GetClass()->ImplementsInterface(USBChatChannelEvent::StaticClass()))
    {
        SBChatManager::Get().SetChannelEvent(InChannelEvent);
    }
    else
    {
        ensureMsgf(false, TEXT("[USBChat::SetChannelEvent] %s is not USBChatChannelEvent!!"), *InChannelEvent->GetName());
    }
}

USBChat* USBChat::Connect(const FString& UserID, const FString& AccessToken)
{
	USBChat* BlueprintAsyncAction = NewObject<USBChat>();
#if WITH_SENDBIRD
	TWeakObjectPtr<USBChat> WeakSBChat = BlueprintAsyncAction;

	SBDMain::Connect(TCHAR_TO_WCHAR(*UserID), TCHAR_TO_WCHAR(*AccessToken), [UserID, AccessToken, WeakSBChat](SBDUser* User, SBDError* Error) {
		ProcessCompletionHandler(WeakSBChat, Error, [User]() {
		});
	});
#endif
	return BlueprintAsyncAction;
}

USBChat* USBChat::Disconnect()
{
	USBChat* BlueprintAsyncAction = NewObject<USBChat>();
#if WITH_SENDBIRD
	TWeakObjectPtr<USBChat> WeakSBChat = BlueprintAsyncAction;

	Cleanup();
	SBDMain::Disconnect([WeakSBChat]() {
		ProcessCompletionHandler(WeakSBChat, nullptr, []() {
		});
	});
#endif
	return BlueprintAsyncAction;
}

void USBChat::Cleanup()
{
	SBChatManager::Get().Reset();
}
//- Common

//+ User
void USBChat::GetCurrentUserInfo(FSBUserInfo& UserInfo, bool& bSucceeded)
{
	SBDUser* CurrentUser = SBDMain::GetCurrentUser();
	bSucceeded = false;

	if (ensure(CurrentUser))
	{
		UserInfo = FSBUserInfo(CurrentUser);
		bSucceeded = true;
	}
}

USBChat* USBChat::UpdateCurrentUserInfo(const FString& NickName, const FString& ProfileUrl)
{
	USBChat* BlueprintAsyncAction = NewObject<USBChat>();
#if WITH_SENDBIRD
	TWeakObjectPtr<USBChat> WeakSBChat = BlueprintAsyncAction;

	// In order to use the API, the option must be turned on in the dashboard.
	SBDMain::UpdateCurrentUserInfo(TCHAR_TO_WCHAR(*NickName), TCHAR_TO_WCHAR(*ProfileUrl), [NickName, ProfileUrl, WeakSBChat](SBDError* Error) {
		ProcessCompletionHandler(WeakSBChat, Error, [NickName, ProfileUrl]() {
		});
	});
#endif
	return BlueprintAsyncAction;
}

void USBChat::RegisterProfileTexture(UObject* WorldContextObject, const FString& ProfileUrl, UTexture2DDynamic* ProfileTexture)
{
	if (!ensure(ProfileTexture))
		return;

	SBChatManager::Get().GetCachedProfileTexture().Add(ProfileUrl, ProfileTexture);
}

UTexture2DDynamic* USBChat::GetProfileTexture(UObject* WorldContextObject, const FString& ProfileUrl)
{
	if (SBChatManager::Get().GetCachedProfileTexture().Contains(ProfileUrl))
		return SBChatManager::Get().GetCachedProfileTexture()[ProfileUrl];

	return nullptr;
}

USBChat* USBChat::GetAllUserList(UObject* WorldContextObject, bool bClearList, TArray<FSBUserInfo>& UserInfos)
{
	USBChat* BlueprintAsyncAction = NewObject<USBChat>();
#if WITH_SENDBIRD
	TWeakObjectPtr<USBChat> WeakSBChat = BlueprintAsyncAction;
	UWorld* World = WorldContextObject->GetWorld();

	if (bClearList)
	{
		// In order to use the API, the option must be turned on in the dashboard.
		SBDUserListQuery* UserListQuery = SBChatManager::Get().CreateAllUserListQuery();
		if (!ensureMsgf(UserListQuery, TEXT("[USBChat::GetAllUserList] CreateAllUserListQuery() failed!!")))
		{
			World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([BlueprintAsyncAction]() {
				BlueprintAsyncAction->OnFail.Broadcast(TEXT("CreateAllUserListQuery() failed!!"), -1);
			}));
			return BlueprintAsyncAction;
		}
		SBChatManager::Get().ResetUserList();
	}

	GetUserList(World, WeakSBChat, UserInfos);
#endif
	return BlueprintAsyncAction;
}


USBChat* USBChat::FindUser(UObject* WorldContextObject, const FString& UserID, FSBUserInfo& UserInfo)
{
	USBChat* BlueprintAsyncAction = NewObject<USBChat>();
#if WITH_SENDBIRD
	TWeakObjectPtr<USBChat> WeakSBChat = BlueprintAsyncAction;

	UWorld* World = WorldContextObject->GetWorld();
	UserInfo.Init();
	if (!ensureMsgf(SBDMain::GetCurrentUser(), TEXT("[USBChat::FindUser] GetCurrentUser() failed!!")))
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([BlueprintAsyncAction]() {
			BlueprintAsyncAction->OnFail.Broadcast(TEXT("GetCurrentUser() failed!!"), -1);
		}));
		return BlueprintAsyncAction;
	}

	if (TCHAR_TO_WCHAR(*UserID) == SBDMain::GetCurrentUser()->user_id)
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([BlueprintAsyncAction]() {
			BlueprintAsyncAction->OnFail.Broadcast(TEXT("You can't enter your ID."), -1);
		}));
		return BlueprintAsyncAction;
	}

	std::vector<std::wstring> UserIds;
	UserIds.push_back(TCHAR_TO_WCHAR(*UserID));

	// In order to use the API, the option must be turned on in the dashboard.
	SBDUserListQuery* UserListQuery = SBDMain::CreateUserListQuery(UserIds);
	if (!ensureMsgf(UserListQuery, TEXT("[USBChat::FindUser] CreateUserListQuery() failed!!")))
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([BlueprintAsyncAction]() {
			BlueprintAsyncAction->OnFail.Broadcast(TEXT("CreateUserListQuery() failed!!"), -1);
		}));
		return BlueprintAsyncAction;
	}

	if (!UserListQuery->has_next)
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([WeakSBChat]() {
			WeakSBChat->OnSuccess.Broadcast(FString(TEXT("")), 0);
		}));
		return BlueprintAsyncAction;
	}

	UserListQuery->LoadNextPage([&UserInfo, World, WeakSBChat](std::vector<SBDUser> Users, SBDError* Error) {
		if (Error == nullptr && Users.size() > 0 && SBChatManager::Get().IsInUserList(Users[0].user_id))
		{
			AsyncTask(ENamedThreads::GameThread, [WeakSBChat]() {
				if (WeakSBChat.IsValid())
					WeakSBChat->OnFail.Broadcast(TEXT("The user is in the current list."), -1);
			});
			return;
		}

		ProcessCompletionHandler(WeakSBChat, Error, [&UserInfo, &Users]() {
			if (Users.size() > 0 && !SBChatManager::Get().IsInUserList(Users[0].user_id))
			{
				UserInfo = FSBUserInfo(Users[0]);
				SBChatManager::Get().GetUserList().Insert(Users[0], 0);
			}
		});
	});
#endif
	return BlueprintAsyncAction;
}
//- User

//+ OpenChannel
USBChat* USBChat::CreateOpenChannel(const FString& Name, FSBChannelInfo& OpenChannelInfo)
{
	USBChat* BlueprintAsyncAction = NewObject<USBChat>();
#if WITH_SENDBIRD
	TWeakObjectPtr<USBChat> WeakSBChat = BlueprintAsyncAction;

	std::vector<std::wstring> user_ids;
	if (SBDMain::GetCurrentUser() != nullptr)
		user_ids.push_back(SBDMain::GetCurrentUser()->user_id);

	// In order to use the API, the option must be turned on in the dashboard.
	SBDOpenChannel::CreateChannel(TCHAR_TO_WCHAR(*Name), TCHAR_TO_WCHAR(TEXT("")), TCHAR_TO_WCHAR(TEXT("")), TCHAR_TO_WCHAR(TEXT("")), user_ids, TCHAR_TO_WCHAR(TEXT("")),
		[WeakSBChat, &OpenChannelInfo](SBDOpenChannel* OpenChannel, SBDError* Error) {
		ProcessCompletionHandler(WeakSBChat, Error, [OpenChannel, &OpenChannelInfo]() {
			SBChatManager::Get().SetCurrentChannel(OpenChannel);
			SBChatManager::Get().GetOpenChannels().Insert(OpenChannel, 0);
			OpenChannelInfo = FSBChannelInfo(OpenChannel);
		});
	});
#endif
	return BlueprintAsyncAction;
}

USBChat* USBChat::UpdateOpenChannel(UObject* WorldContextObject, int Index, const FString& Name, const FString& NewName, FSBChannelInfo& ChannelInfo)
{
	USBChat* BlueprintAsyncAction = NewObject<USBChat>();
#if WITH_SENDBIRD
	TWeakObjectPtr<USBChat> WeakSBChat = BlueprintAsyncAction;

	UWorld* World = WorldContextObject->GetWorld();
	SBDOpenChannel* SelectedChannel = SBChatManager::Get().GetSelectedOpenChannel(Index, Name);
	if (!ensureMsgf(SelectedChannel, TEXT("[USBChat::UpdateOpenChannel] GetSelectedOpenChannel(%d,%s) failed!!"), Index, *Name))
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([BlueprintAsyncAction]() {
			BlueprintAsyncAction->OnFail.Broadcast(TEXT("GetSelectedOpenChannel() failed!!"), -1);
		}));
		return BlueprintAsyncAction;
	}

	std::vector<std::wstring> OperatorUserIds;
	for (SBDUser& User : SelectedChannel->operators)
		OperatorUserIds.push_back(User.user_id);

	SelectedChannel->UpdateChannel(TCHAR_TO_WCHAR(*NewName), SelectedChannel->cover_url, SelectedChannel->data, OperatorUserIds, SelectedChannel->custom_type,
		[SelectedChannel, Index, &ChannelInfo, WeakSBChat](SBDOpenChannel* OpenChannel, SBDError* Error) {
		ProcessCompletionHandler(WeakSBChat, Error, [&ChannelInfo, Index, OpenChannel]() {
			ChannelInfo = FSBChannelInfo(OpenChannel);
			SBChatManager::Get().SetCurrentChannel(OpenChannel);

			if (ensure(SBChatManager::Get().GetOpenChannels().IsValidIndex(Index)))
				SBChatManager::Get().GetOpenChannels()[Index] = OpenChannel;

		});
	});
#endif
	return BlueprintAsyncAction;
}

USBChat* USBChat::DeleteOpenChannel(UObject* WorldContextObject, int Index, const FString& Name)
{
	USBChat* BlueprintAsyncAction = NewObject<USBChat>();
#if WITH_SENDBIRD
	TWeakObjectPtr<USBChat> WeakSBChat = BlueprintAsyncAction;

	UWorld* World = WorldContextObject->GetWorld();
	SBDOpenChannel* SelectedChannel = SBChatManager::Get().GetSelectedOpenChannel(Index, Name);
	if (!ensureMsgf(SelectedChannel, TEXT("[USBChat::DeleteOpenChannel] GetSelectedOpenChannel(%d,%s) failed!!"), Index, *Name))
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([BlueprintAsyncAction]() {
			BlueprintAsyncAction->OnFail.Broadcast(TEXT("GetSelectedOpenChannel() failed!!"), -1);
		}));
		return BlueprintAsyncAction;
	}

	SelectedChannel->DeleteChannel([SelectedChannel, Index, WeakSBChat](SBDError* Error) {
		ProcessCompletionHandler(WeakSBChat, Error, [Index]() {
			SBChatManager::Get().GetOpenChannels().RemoveAt(Index);
		});
	});
#endif
	return BlueprintAsyncAction;
}

USBChat* USBChat::GetOpenChannelList(UObject* WorldContextObject, bool bClearList, TArray<FSBChannelInfo>& OpenChannelInfos)
{
	USBChat* BlueprintAsyncAction = NewObject<USBChat>();
#if WITH_SENDBIRD
	UWorld* World = WorldContextObject->GetWorld();
	if (bClearList)
	{
		SBDOpenChannelListQuery* OpenChannelListQuery = SBChatManager::Get().CreateOpenChannelListQuery();
		if (!ensureMsgf(OpenChannelListQuery, TEXT("[USBChat::GetOpenChannelList] CreateOpenChannelListQuery() failed!!")))
		{
			World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([BlueprintAsyncAction]() {
				BlueprintAsyncAction->OnFail.Broadcast(TEXT("CreateOpenChannelListQuery() failed!!"), -1);
			}));
			return BlueprintAsyncAction;
		}
		SBChatManager::Get().ResetOpenChannels();
	}

	SBDOpenChannelListQuery* OpenChannelListQuery = SBChatManager::Get().GetOpenChannelListQuery();
	if (!ensureMsgf(OpenChannelListQuery, TEXT("[USBChat::GetOpenChannelList] GetOpenChannelListQuery() failed!!")))
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([BlueprintAsyncAction]() {
			BlueprintAsyncAction->OnFail.Broadcast(TEXT("GetOpenChannelListQuery() failed!!"), -1);
		}));
		return BlueprintAsyncAction;
	}

	if (!OpenChannelListQuery->has_next)
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([BlueprintAsyncAction]() {
			BlueprintAsyncAction->OnSuccess.Broadcast(FString(TEXT("")), 0);
		}));
		return BlueprintAsyncAction;
	}

	TWeakObjectPtr<USBChat> WeakSBChat = BlueprintAsyncAction;
	OpenChannelListQuery->LoadNextPage([&OpenChannelInfos, WeakSBChat](std::vector<SBDOpenChannel*> OpenChannels, SBDError* Error) {
		ProcessCompletionHandler(WeakSBChat, Error, [&OpenChannelInfos, &OpenChannels]() {
			OpenChannelInfos.Empty();
			for (SBDOpenChannel* OpenChannel : OpenChannels)
			{
				SBChatManager::Get().GetOpenChannels().Add(OpenChannel);
				OpenChannelInfos.Add(FSBChannelInfo(OpenChannel));
			}
		});
	});
#endif
	return BlueprintAsyncAction;
}

USBChat* USBChat::GetOpenChannelParticipantList(UObject* WorldContextObject, bool bClearList, TArray<FSBUserInfo>& UserList)
{
	USBChat* BlueprintAsyncAction = NewObject<USBChat>();
#if WITH_SENDBIRD
	TWeakObjectPtr<USBChat> WeakSBChat = BlueprintAsyncAction;

	UWorld* World = WorldContextObject->GetWorld();
	SBDBaseChannel* CurrentChannel = SBChatManager::Get().GetCurrentChannel();
	if (!ensureMsgf(CurrentChannel, TEXT("[USBChat::GetOpenChannelParticipantList] GetCurrentChannel() failed!!")))
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([BlueprintAsyncAction]() {
			BlueprintAsyncAction->OnFail.Broadcast(TEXT("GetCurrentChannel() failed!!"), -1);
		}));
		return BlueprintAsyncAction;
	}

	if (!ensure(CurrentChannel->is_open_channel))
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([BlueprintAsyncAction]() {
			BlueprintAsyncAction->OnFail.Broadcast(TEXT("CurrentChannel is not open!!"), -1);
		}));
		return BlueprintAsyncAction;
	}

	SBDOpenChannel* CurrentOpenChannel = static_cast<SBDOpenChannel*>(CurrentChannel);
	if (bClearList)
	{
		// In order to use the API, the option must be turned on in the dashboard.
		SBDUserListQuery* UserListQuery = SBChatManager::Get().CreateParticipantListQuery(CurrentOpenChannel);
		if (!ensureMsgf(UserListQuery, TEXT("[USBChat::GetOpenChannelParticipantList] CreateParticipantListQuery() failed!!")))
		{
			World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([BlueprintAsyncAction]() {
				BlueprintAsyncAction->OnFail.Broadcast(TEXT("CreateParticipantListQuery() failed!!"), -1);
			}));
			return BlueprintAsyncAction;
		}
		SBChatManager::Get().ResetUserList();
	}

	GetUserList(World, WeakSBChat, UserList);
#endif
	return BlueprintAsyncAction;
}

USBChat* USBChat::EnterOpenChannel(UObject* WorldContextObject, int Index, const FString& Name)
{
	USBChat* BlueprintAsyncAction = NewObject<USBChat>();
#if WITH_SENDBIRD
	TWeakObjectPtr<USBChat> WeakSBChat = BlueprintAsyncAction;

	UWorld* World = WorldContextObject->GetWorld();
	SBDOpenChannel* SelectedChannel = SBChatManager::Get().GetSelectedOpenChannel(Index, Name);
	if (!ensureMsgf(SelectedChannel, TEXT("[USBChat::EnterOpenChannel] GetSelectedOpenChannel(%d,%s) failed!!"), Index, *Name))
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([BlueprintAsyncAction]() {
			BlueprintAsyncAction->OnFail.Broadcast(TEXT("GetSelectedOpenChannel() failed!!"), -1);
		}));
		return BlueprintAsyncAction;
	}

	SelectedChannel->Enter([SelectedChannel, Name, WeakSBChat](SBDError* Error) {
		ProcessCompletionHandler(WeakSBChat, Error, [SelectedChannel]() {
			SBChatManager::Get().SetCurrentChannel(SelectedChannel);
		});
	});
#endif
	return BlueprintAsyncAction;
}

USBChat* USBChat::ExitOpenChannel(UObject* WorldContextObject)
{
	USBChat* BlueprintAsyncAction = NewObject<USBChat>();
#if WITH_SENDBIRD
	TWeakObjectPtr<USBChat> WeakSBChat = BlueprintAsyncAction;

	UWorld* World = WorldContextObject->GetWorld();
	SBDOpenChannel* CurrentChannel = static_cast<SBDOpenChannel*>(SBChatManager::Get().GetCurrentChannel());
	if (!ensureMsgf(CurrentChannel, TEXT("[USBChat::ExitOpenChannel] The current channel not exists!!")))
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([BlueprintAsyncAction]() {
			BlueprintAsyncAction->OnFail.Broadcast(TEXT("The current channel not exists!!"), -1);
		}));
		return BlueprintAsyncAction;
	}

	CurrentChannel->Exit([WeakSBChat, CurrentChannel](SBDError* Error) {
		ProcessCompletionHandler(WeakSBChat, Error, []() {
			SBChatManager::Get().ResetCurrentChannel();
		});
	});
#endif
	return BlueprintAsyncAction;
}

USBChat* USBChat::TryToExitOpenChannel(UObject* WorldContextObject)
{
	USBChat* BlueprintAsyncAction = NewObject<USBChat>();
#if WITH_SENDBIRD
	TWeakObjectPtr<USBChat> WeakSBChat = BlueprintAsyncAction;

	UWorld* World = WorldContextObject->GetWorld();
	if (!SBChatManager::Get().GetCurrentChannel() || SBChatManager::Get().GetCurrentChannel()->is_group_channel)
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([BlueprintAsyncAction]() {
			BlueprintAsyncAction->OnSuccess.Broadcast(TEXT(""), 0);
		}));
		return BlueprintAsyncAction;
	}

	SBDOpenChannel* CurrentChannel = static_cast<SBDOpenChannel*>(SBChatManager::Get().GetCurrentChannel());
	CurrentChannel->Exit([WeakSBChat, CurrentChannel](SBDError* Error) {
		ProcessCompletionHandler(WeakSBChat, Error, []() {
			SBChatManager::Get().ResetCurrentChannel();
		});
	});
#endif
	return BlueprintAsyncAction;
}
//- OpenChannel

//+ GroupChannel
USBChat* USBChat::CreateGroupChannel(const FString& Name, FSBChannelInfo& GroupChannelInfo)
{
	USBChat* BlueprintAsyncAction = NewObject<USBChat>();
#if WITH_SENDBIRD
	TWeakObjectPtr<USBChat> WeakSBChat = BlueprintAsyncAction;
	
	std::vector<std::wstring> user_ids;
	if (SBDMain::GetCurrentUser() != nullptr)
		user_ids.push_back(SBDMain::GetCurrentUser()->user_id);

	// In order to use the API, the option must be turned on in the dashboard.
	SBDGroupChannel::CreateChannel(user_ids, TCHAR_TO_WCHAR(*Name), false, TCHAR_TO_WCHAR(TEXT("")), TCHAR_TO_WCHAR(TEXT("")), TCHAR_TO_WCHAR(TEXT("")), [WeakSBChat, &GroupChannelInfo](SBDGroupChannel* GroupChannel, SBDError* Error) {
		ProcessCompletionHandler(WeakSBChat, Error, [GroupChannel, &GroupChannelInfo]() {
			SBChatManager::Get().SetCurrentChannel(GroupChannel);
			SBChatManager::Get().GetGroupChannels().Insert(GroupChannel, 0);
			GroupChannelInfo = FSBChannelInfo(GroupChannel);
		});
	});
#endif
	return BlueprintAsyncAction;
}

USBChat* USBChat::CreateGroupChannelWithUserIds(UObject* WorldContextObject, const FString& ChannelName, const TArray<FString>& FriendIds)
{
	USBChat* BlueprintAsyncAction = NewObject<USBChat>();
#if WITH_SENDBIRD
	TWeakObjectPtr<USBChat> WeakSBChat = BlueprintAsyncAction;
	std::vector<std::wstring> user_ids;
	if (SBDMain::GetCurrentUser() != nullptr)
		user_ids.push_back(SBDMain::GetCurrentUser()->user_id);

	for (const FString& UserId : FriendIds)
		user_ids.push_back(TCHAR_TO_WCHAR(*UserId));

	// In order to use the API, the option must be turned on in the dashboard.
	SBDGroupChannel::CreateChannel(user_ids, TCHAR_TO_WCHAR(*ChannelName), false, TCHAR_TO_WCHAR(TEXT("")), TCHAR_TO_WCHAR(TEXT("")), TCHAR_TO_WCHAR(TEXT("")), [WeakSBChat](SBDGroupChannel* GroupChannel, SBDError* Error) {
		ProcessCompletionHandler(WeakSBChat, Error, [GroupChannel]() {
			SBChatManager::Get().SetCurrentChannel(GroupChannel);
		});
	});
#endif
	return BlueprintAsyncAction;
}

USBChat* USBChat::UpdateGroupChannel(UObject* WorldContextObject, int Index, const FString& Name, const FString& NewName, FSBChannelInfo& ChannelInfo)
{
	USBChat* BlueprintAsyncAction = NewObject<USBChat>();
#if WITH_SENDBIRD
	TWeakObjectPtr<USBChat> WeakSBChat = BlueprintAsyncAction;

	UWorld* World = WorldContextObject->GetWorld();
	SBDGroupChannel* SelectedChannel = SBChatManager::Get().GetSelectedGroupChannel(Index, Name);
	if (!ensureMsgf(SelectedChannel, TEXT("[USBChat::UpdateGroupChannel] GetSelectedGroupChannel(%d,%s) failed!!"), Index, *Name))
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([BlueprintAsyncAction]() {
			BlueprintAsyncAction->OnFail.Broadcast(TEXT("GetSelectedGroupChannel() failed!!"), -1);
		}));
		return BlueprintAsyncAction;
	}

	SelectedChannel->UpdateChannel(TCHAR_TO_WCHAR(*NewName), false, SelectedChannel->cover_url, SelectedChannel->data, SelectedChannel->custom_type,
		[SelectedChannel, Index, WeakSBChat, &ChannelInfo](SBDGroupChannel* GroupChannel, SBDError* Error) {
		ProcessCompletionHandler(WeakSBChat, Error, [Index, GroupChannel, &ChannelInfo]() {
			SBChatManager::Get().SetCurrentChannel(GroupChannel);
			ChannelInfo = FSBChannelInfo(GroupChannel);
			if (ensure(SBChatManager::Get().GetGroupChannels().IsValidIndex(Index)))
				SBChatManager::Get().GetGroupChannels()[Index] = GroupChannel;
		});
	});
#endif
	return BlueprintAsyncAction;
}

USBChat* USBChat::DeleteGroupChannel(UObject* WorldContextObject, int Index, const FString& Name)
{
	USBChat* BlueprintAsyncAction = NewObject<USBChat>();
#if WITH_SENDBIRD
	TWeakObjectPtr<USBChat> WeakSBChat = BlueprintAsyncAction;

	UWorld* World = WorldContextObject->GetWorld();
	SBDGroupChannel* SelectedChannel = SBChatManager::Get().GetSelectedGroupChannel(Index, Name);
	if (!ensureMsgf(SelectedChannel, TEXT("[USBChat::DeleteGroupChannel] GetSelectedGroupChannel(%d,%s) failed!!"), Index, *Name))
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([BlueprintAsyncAction]() {
			BlueprintAsyncAction->OnFail.Broadcast(TEXT("GetSelectedGroupChannel() failed!!"), -1);
		}));
		return BlueprintAsyncAction;
	}

	for (SBDMember& Member : SelectedChannel->members)
	{
		if (SBDMain::GetCurrentUser() != nullptr && Member.user_id == SBDMain::GetCurrentUser()->user_id)
		{
			SelectedChannel->LeaveChannel([Index, WeakSBChat](SBDError* Error) {
				ProcessCompletionHandler(WeakSBChat, Error, [Index]() {
					SBChatManager::Get().ResetCurrentChannel();
					SBChatManager::Get().GetGroupChannels().RemoveAt(Index);
				});
			});
			return BlueprintAsyncAction;
		}
	}

	SelectedChannel->DeleteChannel([SelectedChannel, Index, WeakSBChat](SBDError* Error) {
		ProcessCompletionHandler(WeakSBChat, Error, [Index]() {
			SBChatManager::Get().GetGroupChannels().RemoveAt(Index);
		});
	});
#endif
	return BlueprintAsyncAction;
}

USBChat* USBChat::GetGroupChannelList(UObject* WorldContextObject, bool bClearList, TArray<struct FSBChannelInfo>& GroupChannelInfos)
{
	USBChat* BlueprintAsyncAction = NewObject<USBChat>();
#if WITH_SENDBIRD
	UWorld* World = WorldContextObject->GetWorld();
	if (bClearList)
	{
		SBDGroupChannelListQuery* GroupChannelListQuery = SBChatManager::Get().CreateGroupChannelListQuery();
		if (!ensureMsgf(GroupChannelListQuery, TEXT("[USBChat::GetGroupChannelList] CreateGroupChannelListQuery() failed!!")))
		{
			World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([BlueprintAsyncAction]() {
				BlueprintAsyncAction->OnFail.Broadcast(TEXT("CreateGroupChannelListQuery() failed!!"), -1);
			}));
			return BlueprintAsyncAction;
		}
		SBChatManager::Get().ResetGroupChannels();
	}

	SBDGroupChannelListQuery* GroupChannelListQuery = SBChatManager::Get().GetGroupChannelListQuery();
	if (!ensureMsgf(GroupChannelListQuery, TEXT("[USBChat::GetGroupChannelList] GetGroupChannelListQuery() failed!!")))
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([BlueprintAsyncAction]() {
			BlueprintAsyncAction->OnFail.Broadcast(TEXT("GetGroupChannelListQuery() failed!!"), -1);
		}));
		return BlueprintAsyncAction;
	}

	if (!GroupChannelListQuery->has_next)
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([BlueprintAsyncAction]() {
			BlueprintAsyncAction->OnSuccess.Broadcast(FString(TEXT("")), 0);
		}));
		return BlueprintAsyncAction;
	}

	TWeakObjectPtr<USBChat> WeakSBChat = BlueprintAsyncAction;
	GroupChannelListQuery->LoadNextPage([&GroupChannelInfos, WeakSBChat](std::vector<SBDGroupChannel*> GroupChannels, SBDError* Error) {
		ProcessCompletionHandler(WeakSBChat, Error, [&GroupChannelInfos, &GroupChannels]() {
			GroupChannelInfos.Empty();
			for (SBDGroupChannel* GroupChannel : GroupChannels)
			{
				SBChatManager::Get().GetGroupChannels().Add(GroupChannel);
				GroupChannelInfos.Add(FSBChannelInfo(GroupChannel));
			}
		});
	});
#endif
	return BlueprintAsyncAction;
}

USBChat* USBChat::JoinGroupChannel(UObject* WorldContextObject, int Index, const FString& Name, TArray<FSBUserInfo>& Members)
{
	USBChat* BlueprintAsyncAction = NewObject<USBChat>();
#if WITH_SENDBIRD
	TWeakObjectPtr<USBChat> WeakSBChat = BlueprintAsyncAction;

	UWorld* World = WorldContextObject->GetWorld();
	SBDGroupChannel* SelectedChannel = SBChatManager::Get().GetSelectedGroupChannel(Index, Name);
	if (!ensureMsgf(SelectedChannel, TEXT("[USBChat::JoinGroupChannel] GetSelectedGroupChannel(%d,%s) failed!!"), Index, *Name))
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([BlueprintAsyncAction]() {
			BlueprintAsyncAction->OnFail.Broadcast(TEXT("GetSelectedGroupChannel() failed!!"), -1);
		}));
		return BlueprintAsyncAction;
	}

	bool bAlreadyInclude = false;
	for (SBDMember& Member : SelectedChannel->members)
	{
		SBChatManager::Get().ResetUserList();
		const FSBUserInfo UesrInfo(Member);
		SBChatManager::Get().GetUserList().Add(Member);
		Members.Add(UesrInfo);
		if (SBDMain::GetCurrentUser() != nullptr && Member.user_id == SBDMain::GetCurrentUser()->user_id)
			bAlreadyInclude = true;
	}

	if (bAlreadyInclude)
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([SelectedChannel, BlueprintAsyncAction]() {
			SBChatManager::Get().SetCurrentChannel(SelectedChannel);
			BlueprintAsyncAction->OnSuccess.Broadcast(TEXT(""), 0);
		}));
		return BlueprintAsyncAction;
	}

	SelectedChannel->JoinChannel([SelectedChannel, Name, WeakSBChat, &Members](SBDError* Error) {
		ProcessCompletionHandler(WeakSBChat, Error, [SelectedChannel, &Members]() {
			if (SBDMain::GetCurrentUser() != nullptr)
			{
				FSBUserInfo UserInfo(SBDMain::GetCurrentUser());
				Members.Add(UserInfo);
				SBChatManager::Get().GetUserList().Add(*SBDMain::GetCurrentUser());
			}
			SBChatManager::Get().SetCurrentChannel(SelectedChannel);
		});

	});
#endif
	return BlueprintAsyncAction;
}

USBChat* USBChat::LeaveGroupChannel(UObject* WorldContextObject)
{
	USBChat* BlueprintAsyncAction = NewObject<USBChat>();
#if WITH_SENDBIRD
	TWeakObjectPtr<USBChat> WeakSBChat = BlueprintAsyncAction;

	UWorld* World = WorldContextObject->GetWorld();
	if (!ensure(SBChatManager::Get().GetCurrentChannel()))
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([BlueprintAsyncAction]() {
			BlueprintAsyncAction->OnFail.Broadcast(TEXT("CurrentChannel is null."), -1);
		}));
		return BlueprintAsyncAction;
	}

	if (!ensureMsgf(SBChatManager::Get().GetCurrentChannel()->is_group_channel, TEXT("[USBChat::LeaveGroupChannel] CurrentChannel is not group.!!")))
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([BlueprintAsyncAction]() {
			BlueprintAsyncAction->OnFail.Broadcast(TEXT("CurrentChannel is not group."), -1);
		}));
		return BlueprintAsyncAction;
	}

	SBDGroupChannel* CurrentChannel = static_cast<SBDGroupChannel*>(SBChatManager::Get().GetCurrentChannel());
	if (!ensureMsgf(CurrentChannel, TEXT("[USBChat::LeaveGroupChannel] Not Exist CurrentChannel!!")))
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([BlueprintAsyncAction]() {
			BlueprintAsyncAction->OnFail.Broadcast(TEXT("Not Exist CurrentChannel!!"), -1);
		}));
		return BlueprintAsyncAction;
	}

	CurrentChannel->LeaveChannel([CurrentChannel, WeakSBChat](SBDError* Error) {
		ProcessCompletionHandler(WeakSBChat, Error, [CurrentChannel]() {
			SBChatManager::Get().ResetCurrentChannel();
			SBChatManager::Get().GetGroupChannels().Remove(CurrentChannel);
		});
	});
#endif
	return BlueprintAsyncAction;
}
//- GroupChannel

//+ Message
USBChat* USBChat::SendUserMessage(UObject* WorldContextObject, const FString& SendMessage, FSBMessageInfo& MessageInfo)
{
	USBChat* BlueprintAsyncAction = NewObject<USBChat>();
#if WITH_SENDBIRD
	TWeakObjectPtr<USBChat> WeakSBChat = BlueprintAsyncAction;

	UWorld* World = WorldContextObject->GetWorld();
	SBDBaseChannel* CurrentChannel = SBChatManager::Get().GetCurrentChannel();
	if (!ensureMsgf(CurrentChannel, TEXT("[USBChat::SendUserMessage] Wrong CurrentChannel!!")))
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([BlueprintAsyncAction]() {
			BlueprintAsyncAction->OnFail.Broadcast(TEXT("Wrong CurrentChannel!!"), -1);
		}));
		return BlueprintAsyncAction;
	}

	SBDUserMessageParams Params;
	Params.SetMessage(TCHAR_TO_WCHAR(*SendMessage));
	CurrentChannel->SendUserMessage(Params, [WeakSBChat, &MessageInfo](SBDUserMessage* UserMessage, SBDError* Error) {
		ProcessCompletionHandler(WeakSBChat, Error, [&MessageInfo, &UserMessage]() {
			SBDBaseChannel* CurrentChannel = SBChatManager::Get().GetCurrentChannel();
			if (CurrentChannel && CurrentChannel->is_group_channel)
			{
				SBDGroupChannel* GroupChannel = static_cast<SBDGroupChannel*>(CurrentChannel);
				GroupChannel->MarkAsRead();
			}
			MessageInfo = SBChatManager::Get().AddHistoryMessage(UserMessage);
		});
	});
#endif
	return BlueprintAsyncAction;
}

USBChat* USBChat::UpdateUserMessage(UObject* WorldContextObject, int64 MessageID, const FString& NewMessage, FSBMessageInfo& MessageInfo)
{
	USBChat* BlueprintAsyncAction = NewObject<USBChat>();
#if WITH_SENDBIRD
	TWeakObjectPtr<USBChat> WeakSBChat = BlueprintAsyncAction;

	UWorld* World = WorldContextObject->GetWorld();
	SBDBaseChannel* CurrentChannel = SBChatManager::Get().GetCurrentChannel();
	if (!ensureMsgf(CurrentChannel, TEXT("[USBChat::UpdateUserMessage] GetCurrentChannel() failed!!")))
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([BlueprintAsyncAction]() {
			BlueprintAsyncAction->OnFail.Broadcast(TEXT("GetCurrentChannel() failed!!"), -1);
		}));
		return BlueprintAsyncAction;
	}

	SBDBaseMessage* Message = SBChatManager::Get().GetHistoryMessage(MessageID);
	if (!ensureMsgf(Message, TEXT("[USBChat::UpdateUserMessage] MessageID(%d) is wrong."), MessageID))
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([BlueprintAsyncAction]() {
			BlueprintAsyncAction->OnFail.Broadcast(TEXT("MessageID is wrong."), -1);
		}));
		return BlueprintAsyncAction;
	}

	SBDUserMessage* UserMessage = static_cast<SBDUserMessage*>(Message);
	CurrentChannel->UpdateUserMessage(UserMessage, TCHAR_TO_WCHAR(*NewMessage), UserMessage->data, UserMessage->custom_type, [&MessageInfo, WeakSBChat](SBDUserMessage* NewUserMessage, SBDError* Error) {
		ProcessCompletionHandler(WeakSBChat, Error, [&MessageInfo, NewUserMessage]() {
			MessageInfo = SBChatManager::Get().UpdateHistoryMessage(NewUserMessage);
		});
	});
#endif
	return BlueprintAsyncAction;
}

USBChat* USBChat::DeleteMessage(UObject* WorldContextObject, int64 MessageID)
{
	USBChat* BlueprintAsyncAction = NewObject<USBChat>();
#if WITH_SENDBIRD
	TWeakObjectPtr<USBChat> WeakSBChat = BlueprintAsyncAction;

	UWorld* World = WorldContextObject->GetWorld();
	SBDBaseChannel* CurrentChannel = SBChatManager::Get().GetCurrentChannel();
	if (!ensureMsgf(CurrentChannel, TEXT("[USBChat::DeleteMessage] GetCurrentChannel() failed!!")))
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([BlueprintAsyncAction]() {
			BlueprintAsyncAction->OnFail.Broadcast(TEXT("GetCurrentChannel() failed!!"), -1);
		}));
		return BlueprintAsyncAction;
	}

	SBDBaseMessage* Message = SBChatManager::Get().GetHistoryMessage(MessageID);
	if (!ensureMsgf(Message, TEXT("[USBChat::DeleteMessage] MessageID(%d) is wrong."), MessageID))
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([BlueprintAsyncAction]() {
			BlueprintAsyncAction->OnFail.Broadcast(TEXT("MessageID is wrong."), -1);
		}));
		return BlueprintAsyncAction;
	}

	CurrentChannel->DeleteMessage(Message, [MessageID, Message, WeakSBChat](SBDError* Error) {
		ProcessCompletionHandler(WeakSBChat, Error, [Message, MessageID]() {
			SBChatManager::Get().DeleteHistoryMessage(MessageID);
		});
	});
#endif
	return BlueprintAsyncAction;
}

USBChat* USBChat::GetPreviousMessageList(UObject* WorldContextObject, TArray<FSBMessageInfo>& MessageInfos)
{
	USBChat* BlueprintAsyncAction = NewObject<USBChat>();
#if WITH_SENDBIRD
	TWeakObjectPtr<USBChat> WeakSBChat = BlueprintAsyncAction;

	UWorld* World = WorldContextObject->GetWorld();
	SBDBaseChannel* CurrentChannel = SBChatManager::Get().GetCurrentChannel();
	if (!ensureMsgf(CurrentChannel, TEXT("[USBChat::GetPreviousMessageList] Wrong CurrentChannel!!")))
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([BlueprintAsyncAction]() {
			BlueprintAsyncAction->OnFail.Broadcast(TEXT("Wrong CurrentChannel!!"), -1);
		}));
		return BlueprintAsyncAction;
	}

	SBDPreviousMessageListQuery* ListQuery = CurrentChannel->CreatePreviousMessageListQuery();
	ListQuery->LoadNextPage(SBChatManager::MESSAGE_QUERY_LIST_LIMIT, false, [&MessageInfos, WeakSBChat](std::vector<SBDBaseMessage*> Messages, SBDError* Error) {
		ProcessCompletionHandler(WeakSBChat, Error, [&MessageInfos, &Messages]() {
			SBDBaseChannel* CurrentChannel = SBChatManager::Get().GetCurrentChannel();
			if (CurrentChannel && CurrentChannel->is_group_channel)
			{
				SBDGroupChannel* GroupChannel = static_cast<SBDGroupChannel*>(CurrentChannel);
				GroupChannel->MarkAsRead();
			}

			MessageInfos.Empty();
			SBChatManager::Get().ResetHistoryMessage();
			for (SBDBaseMessage* Message : Messages)
			{
				const FSBMessageInfo AddedMessage = SBChatManager::Get().AddHistoryMessage(Message);
				MessageInfos.Add(AddedMessage);
			}
		});
	});
#endif
	return BlueprintAsyncAction;
}
//- Message

//+ private
void USBChat::GetUserList(UWorld* World, TWeakObjectPtr<USBChat> WeakSBChat, TArray<FSBUserInfo>& UserList)
{
#if WITH_SENDBIRD
	// In order to use the API, the option must be turned on in the dashboard.
	SBDUserListQuery* UserListQuery = SBChatManager::Get().GetUserListQuery();

	if (!ensureMsgf(UserListQuery, TEXT("[USBChat::GetUserList] GetUserListQuery() failed!!")))
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([WeakSBChat]() {
			WeakSBChat->OnFail.Broadcast(TEXT("GetUserListQuery() failed!!"), -1);
		}));
		return;
	}

	if (!UserListQuery->has_next)
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([WeakSBChat]() {
			WeakSBChat->OnSuccess.Broadcast(FString(TEXT("")), 0);
		}));
		return;
	}

	UserListQuery->LoadNextPage([&UserList, WeakSBChat](std::vector<SBDUser> Users, SBDError* Error) {
		ProcessCompletionHandler(WeakSBChat, Error, [&UserList, &Users]() {
			UserList.Empty();
			for (SBDUser& User : Users)
			{
				SBChatManager::Get().GetUserList().Add(User);
				UserList.Add(FSBUserInfo(User));
			}
		});
	});
#endif
}

void USBChat::ProcessCompletionHandler(TWeakObjectPtr<USBChat> WeakSBChat, SBDError* Error, std::function<void()> SuccessHandler)
{
	if (Error != nullptr) {
		FString ErrorMessage = WCHAR_TO_TCHAR(Error->message.c_str());
		int64 ErrorCode = Error->code;
		UE_LOG(SendbirdSample, Error, TEXT("[USBChat::CompletionHandler] ErrorMessage(%s) ErrorCode(%d)"), *ErrorMessage, ErrorCode);

		AsyncTask(ENamedThreads::GameThread, [WeakSBChat, ErrorMessage, ErrorCode]() {
			if (WeakSBChat.IsValid())
				WeakSBChat->OnFail.Broadcast(ErrorMessage, ErrorCode);
		});
	}
	else
	{
		SuccessHandler();

		AsyncTask(ENamedThreads::GameThread, [WeakSBChat]() {
			if (WeakSBChat.IsValid())
				WeakSBChat->OnSuccess.Broadcast(FString(TEXT("")), 0);
		});
	}
}
//- private
