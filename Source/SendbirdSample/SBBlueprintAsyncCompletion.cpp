// Copyright (c) 2021 Sendbird, Inc. All rights reserved.

#include "SBBlueprintAsyncCompletion.h"
#include "Kismet/GameplayStatics.h"
#include "Sendbird/include/Sendbird.h"
#include "SendbirdSample.h"
#include "SendbirdChatManager.h"
#include "SBChannelInterface.h"
#include "SendbirdSampleGameMode.h"

#if PLATFORM_ANDROID
#include "Android/AndroidPlatformFile.h"
#endif

USBBlueprintAsyncCompletion::USBBlueprintAsyncCompletion(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FString USBBlueprintAsyncCompletion::Sendbird_GetSdkVersion() {
	return SendbirdChatMananger::Get().GetSdkVersion();
}

bool USBBlueprintAsyncCompletion::Sendbird_Init(const FString& AppID)
{
	return SendbirdChatMananger::Get().Initialize(AppID);
}

void USBBlueprintAsyncCompletion::Sendbird_SetChannelInterface(UObject* InChannelInterface)
{
	if (!ensure(InChannelInterface))
		return;

	if (InChannelInterface->GetClass()->ImplementsInterface(USBChannelInterface::StaticClass()))
	{
		SendbirdChatMananger::Get().SetChannelInterface(InChannelInterface);
	}
	else
	{
		ensureMsgf(false, TEXT("[USBBlueprintAsyncCompletion::Sendbird_SetChannelInterface] %s is not USBChannelInterface!!"), *InChannelInterface->GetName());
	}
}

USBBlueprintAsyncCompletion* USBBlueprintAsyncCompletion::Sendbird_Connect(const FString& UserID, const FString& AccessToken, FSBUserInfo& LoginUserInfo)
{
	USBBlueprintAsyncCompletion* AsyncCompletion = NewObject<USBBlueprintAsyncCompletion>();
#if WITH_SENDBIRD
	TWeakObjectPtr<USBBlueprintAsyncCompletion> WeakAsyncCompletion = AsyncCompletion;

	SBDMain::Connect(TCHAR_TO_WCHAR(*UserID), TCHAR_TO_WCHAR(*AccessToken), [UserID, AccessToken, WeakAsyncCompletion, &LoginUserInfo](SBDUser* User, SBDError* Error) {
		ProcessCompletionHandler(WeakAsyncCompletion, Error, [User, &LoginUserInfo]() {
			LoginUserInfo = FSBUserInfo(User);
			SendbirdChatMananger::Get().SetCurrentUser(User);
		});
	});
#endif
	return AsyncCompletion;
}

USBBlueprintAsyncCompletion* USBBlueprintAsyncCompletion::Sendbird_Disconnect()
{
	USBBlueprintAsyncCompletion* AsyncCompletion = NewObject<USBBlueprintAsyncCompletion>();
#if WITH_SENDBIRD
	TWeakObjectPtr<USBBlueprintAsyncCompletion> WeakAsyncCompletion = AsyncCompletion;

	SendbirdChatMananger::Get().Reset();
	SBDMain::Disconnect([WeakAsyncCompletion]() {
		ProcessCompletionHandler(WeakAsyncCompletion, nullptr, []() {
		});
	});
#endif
	return AsyncCompletion;
}

void USBBlueprintAsyncCompletion::Sendbird_Cleanup()
{
	SendbirdChatMananger::Get().Reset();
}

void USBBlueprintAsyncCompletion::Sendbird_GetCurrentUserInfo(FSBUserInfo& UserInfo, bool& bSuccessed)
{
	bSuccessed = true;
	SBDUser* CurrentUser = SendbirdChatMananger::Get().GetCurrentUser();

	if (ensure(CurrentUser))
		UserInfo = FSBUserInfo(CurrentUser);
	else
		bSuccessed = false;
}

USBBlueprintAsyncCompletion* USBBlueprintAsyncCompletion::Sendbird_UpdateCurrentUserInfo(const FString& NickName, const FString& ProfileUrl)
{
	USBBlueprintAsyncCompletion* AsyncCompletion = NewObject<USBBlueprintAsyncCompletion>();
#if WITH_SENDBIRD
	TWeakObjectPtr<USBBlueprintAsyncCompletion> WeakAsyncCompletion = AsyncCompletion;

	SBDMain::UpdateCurrentUserInfo(TCHAR_TO_WCHAR(*NickName), TCHAR_TO_WCHAR(*ProfileUrl), [NickName, ProfileUrl, WeakAsyncCompletion](SBDError* Error) {
		ProcessCompletionHandler(WeakAsyncCompletion, Error, [NickName, ProfileUrl]() {
			// 성공시 정보 갱신
			SendbirdChatMananger::Get().GetCurrentUser()->nickname = TCHAR_TO_WCHAR(*NickName);
			SendbirdChatMananger::Get().GetCurrentUser()->profile_url = TCHAR_TO_WCHAR(*ProfileUrl);
		});
	});
#endif
	return AsyncCompletion;
}

USBBlueprintAsyncCompletion* USBBlueprintAsyncCompletion::Sendbird_GetFriendList(UObject* WorldContextObject, bool bClearList, TArray<struct FSBUserInfo>& FriendInfos)
{
	USBBlueprintAsyncCompletion* AsyncCompletion = NewObject<USBBlueprintAsyncCompletion>();
#if WITH_SENDBIRD
	TWeakObjectPtr<USBBlueprintAsyncCompletion> WeakAsyncCompletion = AsyncCompletion;
	UWorld* MyWorld = WorldContextObject->GetWorld();

	if (bClearList)
	{
		SBDUserListQuery* UserListQuery = SendbirdChatMananger::Get().CreateAllUserListQuery();
		if (!ensureMsgf(UserListQuery, TEXT("[USBBlueprintAsyncCompletion::Sendbird_GetFriendList] CreateAllUserListQuery() failed!!")))
		{
			MyWorld->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([AsyncCompletion]() {
				AsyncCompletion->OnFail.Broadcast(TEXT("CreateAllUserListQuery() failed!!"), -1);
			}));
			return AsyncCompletion;
		}
		SendbirdChatMananger::Get().ResetUserList();
	}
	GetUserList(MyWorld, WeakAsyncCompletion, FriendInfos);
#endif
	return AsyncCompletion;
}


USBBlueprintAsyncCompletion* USBBlueprintAsyncCompletion::Sendbird_FindUser(UObject* WorldContextObject, const FString& UserID, FSBUserInfo& UserInfo)
{
	USBBlueprintAsyncCompletion* AsyncCompletion = NewObject<USBBlueprintAsyncCompletion>();
#if WITH_SENDBIRD
	TWeakObjectPtr<USBBlueprintAsyncCompletion> WeakAsyncCompletion = AsyncCompletion;

	UWorld* World = WorldContextObject->GetWorld();
	UserInfo.Init();
	if (!ensureMsgf(SendbirdChatMananger::Get().GetCurrentUser(), TEXT("[USBBlueprintAsyncCompletion::Sendbird_FindUser] GetCurrentUser() failed!!")))
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([AsyncCompletion]() {
			AsyncCompletion->OnFail.Broadcast(TEXT("GetCurrentUser() failed!!"), -1);
		}));
		return AsyncCompletion;
	}

	if (TCHAR_TO_WCHAR(*UserID) == SendbirdChatMananger::Get().GetCurrentUser()->user_id)
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([AsyncCompletion]() {
			AsyncCompletion->OnFail.Broadcast(TEXT("You can't enter your ID."), -1);
		}));
		return AsyncCompletion;
	}

	std::vector<std::wstring> UserIds;
	UserIds.push_back(TCHAR_TO_WCHAR(*UserID));
	SBDUserListQuery* UserListQuery = SBDMain::CreateUserListQuery(UserIds);
	if (!ensureMsgf(UserListQuery, TEXT("[USBBlueprintAsyncCompletion::Sendbird_FindUser] CreateUserListQuery() failed!!")))
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([AsyncCompletion]() {
			AsyncCompletion->OnFail.Broadcast(TEXT("CreateUserListQuery() failed!!"), -1);
		}));
		return AsyncCompletion;
	}

	if (!UserListQuery->has_next)
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([WeakAsyncCompletion]() {
			WeakAsyncCompletion->OnSuccess.Broadcast(FString(TEXT("")), 0);
		}));
		return AsyncCompletion;
	}

	UserListQuery->LoadNextPage([&UserInfo, World, WeakAsyncCompletion](std::vector<SBDUser> Users, SBDError* Error) {
		if (Error == nullptr && Users.size() > 0 && SendbirdChatMananger::Get().ContainsUserList(Users[0].user_id))
		{
			AsyncTask(ENamedThreads::GameThread, [WeakAsyncCompletion]() {
				if (WeakAsyncCompletion.IsValid())
					WeakAsyncCompletion->OnFail.Broadcast(TEXT("The user is in the current list."), -1);
			});
			return;
		}

		ProcessCompletionHandler(WeakAsyncCompletion, Error, [&UserInfo, &Users]() {
			if (Users.size() > 0 && !SendbirdChatMananger::Get().ContainsUserList(Users[0].user_id))
			{
				UserInfo = FSBUserInfo(Users[0]);
				SendbirdChatMananger::Get().GetUserList().Insert(Users[0], 0);
			}
		});
	});
#endif
	return AsyncCompletion;
}

void USBBlueprintAsyncCompletion::Sendbird_RegisterProfileTexture(UObject* WorldContextObject, const FString& ProfileUrl, UTexture2DDynamic* ProfileTexture)
{
	if (!ensure(ProfileTexture))
		return;

	ASendbirdSampleGameMode* GameMode = Cast<ASendbirdSampleGameMode>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (!ensure(GameMode))
		return;

	GameMode->CachedProfileTexture.Add(ProfileUrl, ProfileTexture);
}

UTexture2DDynamic* USBBlueprintAsyncCompletion::Sendbird_GetProfileTexture(UObject* WorldContextObject, const FString& ProfileUrl)
{
	ASendbirdSampleGameMode* GameMode = Cast<ASendbirdSampleGameMode>(UGameplayStatics::GetGameMode(WorldContextObject));

	if (!ensure(GameMode))
		return nullptr;

	if (GameMode->CachedProfileTexture.Contains(ProfileUrl))
		return GameMode->CachedProfileTexture[ProfileUrl];

	return nullptr;
}

USBBlueprintAsyncCompletion* USBBlueprintAsyncCompletion::Sendbird_CreateOpenChannel(const FString& Name, FSBChannelInfo& OpenChannelInfo)
{
	USBBlueprintAsyncCompletion* AsyncCompletion = NewObject<USBBlueprintAsyncCompletion>();
#if WITH_SENDBIRD
	TWeakObjectPtr<USBBlueprintAsyncCompletion> WeakAsyncCompletion = AsyncCompletion;

	std::vector<std::wstring> user_ids;
	user_ids.push_back(SendbirdChatMananger::Get().GetCurrentUser()->user_id);

	SBDOpenChannel::CreateChannel(TCHAR_TO_WCHAR(*Name), TCHAR_TO_WCHAR(TEXT("")), TCHAR_TO_WCHAR(TEXT("")), TCHAR_TO_WCHAR(TEXT("")), user_ids, TCHAR_TO_WCHAR(TEXT("")), [WeakAsyncCompletion, &OpenChannelInfo](SBDOpenChannel* OpenChannel, SBDError* Error) {
		ProcessCompletionHandler(WeakAsyncCompletion, Error, [OpenChannel, &OpenChannelInfo]() {
			SendbirdChatMananger::Get().SetCurrentChannel(OpenChannel);
			SendbirdChatMananger::Get().GetOpenChannels().Insert(OpenChannel, 0);
			OpenChannelInfo = FSBChannelInfo(OpenChannel);
		});
	});
#endif
	return AsyncCompletion;
}

USBBlueprintAsyncCompletion* USBBlueprintAsyncCompletion::Sendbird_UpdateOpenChannel(UObject* WorldContextObject, int Index, const FString& Name, const FString& NewName, FSBChannelInfo& ChannelInfo)
{
	USBBlueprintAsyncCompletion* AsyncCompletion = NewObject<USBBlueprintAsyncCompletion>();
#if WITH_SENDBIRD
	TWeakObjectPtr<USBBlueprintAsyncCompletion> WeakAsyncCompletion = AsyncCompletion;

	UWorld* World = WorldContextObject->GetWorld();
	SBDOpenChannel* SelectedChannel = SendbirdChatMananger::Get().GetSelectedOpenChannel(Index, Name);
	if (!ensureMsgf(SelectedChannel, TEXT("[USBBlueprintAsyncCompletion::Sendbird_UpdateOpenChannel] GetSelectedOpenChannel(%d,%s) failed!!"), Index, *Name))
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([AsyncCompletion]() {
			AsyncCompletion->OnFail.Broadcast(TEXT("GetSelectedOpenChannel() failed!!"), -1);
		}));
		return AsyncCompletion;
	}

	std::vector<std::wstring> OperatorUserIds;
	for (SBDUser& User : SelectedChannel->operators)
		OperatorUserIds.push_back(User.user_id);

	SelectedChannel->UpdateChannel(TCHAR_TO_WCHAR(*NewName), SelectedChannel->cover_url, SelectedChannel->data, OperatorUserIds, SelectedChannel->custom_type,
		[SelectedChannel, Index, &ChannelInfo, WeakAsyncCompletion](SBDOpenChannel* OpenChannel, SBDError* Error) {
		ProcessCompletionHandler(WeakAsyncCompletion, Error, [&ChannelInfo, Index, OpenChannel]() {
			ChannelInfo = FSBChannelInfo(OpenChannel);
			SendbirdChatMananger::Get().SetCurrentChannel(OpenChannel);

			if (ensure(SendbirdChatMananger::Get().GetOpenChannels().IsValidIndex(Index)))
				SendbirdChatMananger::Get().GetOpenChannels()[Index] = OpenChannel;

		});
	});
#endif
	return AsyncCompletion;
}

USBBlueprintAsyncCompletion* USBBlueprintAsyncCompletion::Sendbird_DeleteOpenChannel(UObject* WorldContextObject, int Index, const FString& Name)
{
	USBBlueprintAsyncCompletion* AsyncCompletion = NewObject<USBBlueprintAsyncCompletion>();
#if WITH_SENDBIRD
	TWeakObjectPtr<USBBlueprintAsyncCompletion> WeakAsyncCompletion = AsyncCompletion;

	UWorld* World = WorldContextObject->GetWorld();
	SBDOpenChannel* SelectedChannel = SendbirdChatMananger::Get().GetSelectedOpenChannel(Index, Name);
	if (!ensureMsgf(SelectedChannel, TEXT("[USBBlueprintAsyncCompletion::Sendbird_DeleteOpenChannel] GetSelectedOpenChannel(%d,%s) failed!!"), Index, *Name))
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([AsyncCompletion]() {
			AsyncCompletion->OnFail.Broadcast(TEXT("GetSelectedOpenChannel() failed!!"), -1);
		}));
		return AsyncCompletion;
	}

	SelectedChannel->DeleteChannel([SelectedChannel, Index, WeakAsyncCompletion](SBDError* Error) {
		ProcessCompletionHandler(WeakAsyncCompletion, Error, [Index]() {
			SendbirdChatMananger::Get().GetOpenChannels().RemoveAt(Index);
		});
	});
#endif
	return AsyncCompletion;
}

USBBlueprintAsyncCompletion* USBBlueprintAsyncCompletion::Sendbird_GetOpenChannelList(UObject* WorldContextObject, bool bClearList, TArray<FSBChannelInfo>& OpenChannelInfos)
{
	USBBlueprintAsyncCompletion* AsyncCompletion = NewObject<USBBlueprintAsyncCompletion>();
#if WITH_SENDBIRD
	UWorld* MyWorld = WorldContextObject->GetWorld();
	if (bClearList)
	{
		SBDOpenChannelListQuery* OpenChannelListQuery = SendbirdChatMananger::Get().CreateOpenChannelListQuery();
		if (!ensureMsgf(OpenChannelListQuery, TEXT("[USBBlueprintAsyncCompletion::Sendbird_GetOpenChannelList] CreateOpenChannelListQuery() failed!!")))
		{
			MyWorld->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([AsyncCompletion]() {
				AsyncCompletion->OnFail.Broadcast(TEXT("CreateOpenChannelListQuery() failed!!"), -1);
			}));
			return AsyncCompletion;
		}
		SendbirdChatMananger::Get().ResetOpenChannels();
	}

	SBDOpenChannelListQuery* OpenChannelListQuery = SendbirdChatMananger::Get().GetOpenChannelListQuery();
	if (!ensureMsgf(OpenChannelListQuery, TEXT("[USBBlueprintAsyncCompletion::Sendbird_GetOpenChannelList] GetOpenChannelListQuery() failed!!")))
	{
		MyWorld->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([AsyncCompletion]() {
			AsyncCompletion->OnFail.Broadcast(TEXT("GetOpenChannelListQuery() failed!!"), -1);
		}));
		return AsyncCompletion;
	}

	if (!OpenChannelListQuery->has_next)
	{
		MyWorld->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([AsyncCompletion]() {
			AsyncCompletion->OnSuccess.Broadcast(FString(TEXT("")), 0);
		}));
		return AsyncCompletion;
	}

	TWeakObjectPtr<USBBlueprintAsyncCompletion> WeakAsyncCompletion = AsyncCompletion;
	OpenChannelListQuery->LoadNextPage([&OpenChannelInfos, WeakAsyncCompletion](std::vector<SBDOpenChannel*> OpenChannels, SBDError* Error) {
		ProcessCompletionHandler(WeakAsyncCompletion, Error, [&OpenChannelInfos, &OpenChannels]() {
			OpenChannelInfos.Empty();
			for (SBDOpenChannel* OpenChannel : OpenChannels)
			{
				SendbirdChatMananger::Get().GetOpenChannels().Add(OpenChannel);
				OpenChannelInfos.Add(FSBChannelInfo(OpenChannel));
			}
		});
	});
#endif
	return AsyncCompletion;
}

USBBlueprintAsyncCompletion* USBBlueprintAsyncCompletion::Sendbird_GetOpenChannelParticipantList(UObject* WorldContextObject, bool bClearList, TArray<FSBUserInfo>& UserList)
{
	USBBlueprintAsyncCompletion* AsyncCompletion = NewObject<USBBlueprintAsyncCompletion>();
#if WITH_SENDBIRD
	TWeakObjectPtr<USBBlueprintAsyncCompletion> WeakAsyncCompletion = AsyncCompletion;

	UWorld* World = WorldContextObject->GetWorld();
	SBDBaseChannel* CurrentChannel = SendbirdChatMananger::Get().GetCurrentChannel();
	if (!ensureMsgf(CurrentChannel, TEXT("[USBBlueprintAsyncCompletion::Sendbird_GetOpenChannelParticipantList] GetCurrentChannel() failed!!")))
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([AsyncCompletion]() {
			AsyncCompletion->OnFail.Broadcast(TEXT("GetCurrentChannel() failed!!"), -1);
		}));
		return AsyncCompletion;
	}

	if (!ensure(CurrentChannel->is_open_channel))
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([AsyncCompletion]() {
			AsyncCompletion->OnFail.Broadcast(TEXT("CurrentChannel is not open!!"), -1);
		}));
		return AsyncCompletion;
	}

	SBDOpenChannel* CurrentOpenChannel = static_cast<SBDOpenChannel*>(CurrentChannel);
	if (bClearList)
	{
		SBDUserListQuery* UserListQuery = SendbirdChatMananger::Get().CreateParticipantListQuery(CurrentOpenChannel);
		if (!ensureMsgf(UserListQuery, TEXT("[USBBlueprintAsyncCompletion::Sendbird_GetOpenChannelParticipantList] CreateParticipantListQuery() failed!!")))
		{
			World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([AsyncCompletion]() {
				AsyncCompletion->OnFail.Broadcast(TEXT("CreateParticipantListQuery() failed!!"), -1);
			}));
			return AsyncCompletion;
		}
		SendbirdChatMananger::Get().ResetUserList();
	}

	GetUserList(World, WeakAsyncCompletion, UserList);
#endif
	return AsyncCompletion;
}

USBBlueprintAsyncCompletion* USBBlueprintAsyncCompletion::Sendbird_EnterOpenChannel(UObject* WorldContextObject, int Index, const FString& Name)
{
	USBBlueprintAsyncCompletion* AsyncCompletion = NewObject<USBBlueprintAsyncCompletion>();
#if WITH_SENDBIRD
	TWeakObjectPtr<USBBlueprintAsyncCompletion> WeakAsyncCompletion = AsyncCompletion;

	UWorld* World = WorldContextObject->GetWorld();
	SBDOpenChannel* SelectedChannel = SendbirdChatMananger::Get().GetSelectedOpenChannel(Index, Name);
	if (!ensureMsgf(SelectedChannel, TEXT("[USBBlueprintAsyncCompletion::Sendbird_EnterOpenChannel] GetSelectedOpenChannel(%d,%s) failed!!"), Index, *Name))
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([AsyncCompletion]() {
			AsyncCompletion->OnFail.Broadcast(TEXT("GetSelectedOpenChannel() failed!!"), -1);
		}));
		return AsyncCompletion;
	}

	SelectedChannel->Enter([SelectedChannel, Name, WeakAsyncCompletion](SBDError* Error) {
		ProcessCompletionHandler(WeakAsyncCompletion, Error, [SelectedChannel]() {
			SendbirdChatMananger::Get().SetCurrentChannel(SelectedChannel);
		});
	});
#endif
	return AsyncCompletion;
}

USBBlueprintAsyncCompletion* USBBlueprintAsyncCompletion::Sendbird_ExitOpenChannel(UObject* WorldContextObject)
{
	USBBlueprintAsyncCompletion* AsyncCompletion = NewObject<USBBlueprintAsyncCompletion>();
#if WITH_SENDBIRD
	TWeakObjectPtr<USBBlueprintAsyncCompletion> WeakAsyncCompletion = AsyncCompletion;

	UWorld* World = WorldContextObject->GetWorld();
	SBDOpenChannel* CurrentChannel = static_cast<SBDOpenChannel*>(SendbirdChatMananger::Get().GetCurrentChannel());
	if (!ensureMsgf(CurrentChannel, TEXT("[USBBlueprintAsyncCompletion::Sendbird_ExitOpenChannel] The current channel not exists!!")))
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([AsyncCompletion]() {
			AsyncCompletion->OnFail.Broadcast(TEXT("The current channel not exists!!"), -1);
		}));
		return AsyncCompletion;
	}

	CurrentChannel->Exit([WeakAsyncCompletion, CurrentChannel](SBDError* Error) {
		ProcessCompletionHandler(WeakAsyncCompletion, Error, []() {
			SendbirdChatMananger::Get().ResetCurrentChannel();
		});
	});
#endif
	return AsyncCompletion;
}

USBBlueprintAsyncCompletion* USBBlueprintAsyncCompletion::TryExitOpenChannel(UObject* WorldContextObject)
{
	USBBlueprintAsyncCompletion* AsyncCompletion = NewObject<USBBlueprintAsyncCompletion>();
#if WITH_SENDBIRD
	TWeakObjectPtr<USBBlueprintAsyncCompletion> WeakAsyncCompletion = AsyncCompletion;

	UWorld* World = WorldContextObject->GetWorld();
	if (!SendbirdChatMananger::Get().GetCurrentChannel() || SendbirdChatMananger::Get().GetCurrentChannel()->is_group_channel)
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([AsyncCompletion]() {
			AsyncCompletion->OnSuccess.Broadcast(TEXT(""), 0);
		}));
		return AsyncCompletion;
	}

	SBDOpenChannel* CurrentChannel = static_cast<SBDOpenChannel*>(SendbirdChatMananger::Get().GetCurrentChannel());
	CurrentChannel->Exit([WeakAsyncCompletion, CurrentChannel](SBDError* Error) {
		ProcessCompletionHandler(WeakAsyncCompletion, Error, []() {
			SendbirdChatMananger::Get().ResetCurrentChannel();
		});
	});
#endif
	return AsyncCompletion;
}

USBBlueprintAsyncCompletion* USBBlueprintAsyncCompletion::Sendbird_LoadChannelChatHistory(UObject* WorldContextObject, TArray<FSBMessageInfo>& MessageInfos)
{
	USBBlueprintAsyncCompletion* AsyncCompletion = NewObject<USBBlueprintAsyncCompletion>();
#if WITH_SENDBIRD
	TWeakObjectPtr<USBBlueprintAsyncCompletion> WeakAsyncCompletion = AsyncCompletion;

	UWorld* World = WorldContextObject->GetWorld();
	SBDBaseChannel* CurrentChannel = SendbirdChatMananger::Get().GetCurrentChannel();
	if (!ensureMsgf(CurrentChannel, TEXT("[USBBlueprintAsyncCompletion::Sendbird_LoadChannelChatHistory] Wrong CurrentChannel!!")))
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([AsyncCompletion]() {
			AsyncCompletion->OnFail.Broadcast(TEXT("Wrong CurrentChannel!!"), -1);
		}));
		return AsyncCompletion;
	}

	SBDPreviousMessageListQuery* ListQuery = CurrentChannel->CreatePreviousMessageListQuery();
	ListQuery->LoadNextPage(SendbirdChatMananger::MESSAGE_QUERY_LIST_LIMIT, false, [&MessageInfos, WeakAsyncCompletion](std::vector<SBDBaseMessage*> Messages, SBDError* Error) {
		ProcessCompletionHandler(WeakAsyncCompletion, Error, [&MessageInfos, &Messages]() {
			SBDBaseChannel* CurrentChannel = SendbirdChatMananger::Get().GetCurrentChannel();
			if (CurrentChannel && CurrentChannel->is_group_channel)
			{
				SBDGroupChannel* GroupChannel = static_cast<SBDGroupChannel*>(CurrentChannel);
				GroupChannel->MarkAsRead();
			}

			MessageInfos.Empty();
			SendbirdChatMananger::Get().ResetHistoryMessage();
			for (SBDBaseMessage* Message : Messages)
			{
				const FSBMessageInfo AddedMessage = SendbirdChatMananger::Get().AddHistoryMessage(Message);
				MessageInfos.Add(AddedMessage);
			}
		});
	});
#endif
	return AsyncCompletion;
}

USBBlueprintAsyncCompletion* USBBlueprintAsyncCompletion::Sendbird_CreateGroupChannel(const FString& Name, FSBChannelInfo& GroupChannelInfo)
{
	USBBlueprintAsyncCompletion* AsyncCompletion = NewObject<USBBlueprintAsyncCompletion>();
#if WITH_SENDBIRD
	TWeakObjectPtr<USBBlueprintAsyncCompletion> WeakAsyncCompletion = AsyncCompletion;
	
	std::vector<std::wstring> user_ids;
	user_ids.push_back(SendbirdChatMananger::Get().GetCurrentUser()->user_id);
	SBDGroupChannel::CreateChannel(user_ids, TCHAR_TO_WCHAR(*Name), false, TCHAR_TO_WCHAR(TEXT("")), TCHAR_TO_WCHAR(TEXT("")), TCHAR_TO_WCHAR(TEXT("")), [WeakAsyncCompletion, &GroupChannelInfo](SBDGroupChannel* GroupChannel, SBDError* Error) {
		ProcessCompletionHandler(WeakAsyncCompletion, Error, [GroupChannel, &GroupChannelInfo]() {
			SendbirdChatMananger::Get().SetCurrentChannel(GroupChannel);
			SendbirdChatMananger::Get().GetGroupChannels().Insert(GroupChannel, 0);
			GroupChannelInfo = FSBChannelInfo(GroupChannel);
		});
	});
#endif
	return AsyncCompletion;
}

USBBlueprintAsyncCompletion* USBBlueprintAsyncCompletion::Sendbird_CreateChannelWithUserIds(UObject* WorldContextObject, const FString& ChannelName, const TArray<FString>& FriendIds)
{
	USBBlueprintAsyncCompletion* AsyncCompletion = NewObject<USBBlueprintAsyncCompletion>();
#if WITH_SENDBIRD
	UWorld* MyWorld = WorldContextObject->GetWorld();

	TWeakObjectPtr<USBBlueprintAsyncCompletion> WeakAsyncCompletion = AsyncCompletion;
	std::vector<std::wstring> user_ids;
	user_ids.push_back(SendbirdChatMananger::Get().GetCurrentUser()->user_id);
	for (const FString& UserId : FriendIds)
		user_ids.push_back(TCHAR_TO_WCHAR(*UserId));

	SBDGroupChannel::CreateChannel(user_ids, TCHAR_TO_WCHAR(*ChannelName), false, TCHAR_TO_WCHAR(TEXT("")), TCHAR_TO_WCHAR(TEXT("")), TCHAR_TO_WCHAR(TEXT("")), [WeakAsyncCompletion](SBDGroupChannel* GroupChannel, SBDError* Error) {
		ProcessCompletionHandler(WeakAsyncCompletion, Error, [GroupChannel]() {
			SendbirdChatMananger::Get().SetCurrentChannel(GroupChannel);
		});
	});
#endif
	return AsyncCompletion;
}

USBBlueprintAsyncCompletion* USBBlueprintAsyncCompletion::Sendbird_UpdateGroupChannel(UObject* WorldContextObject, int Index, const FString& Name, const FString& NewName, FSBChannelInfo& ChannelInfo)
{
	USBBlueprintAsyncCompletion* AsyncCompletion = NewObject<USBBlueprintAsyncCompletion>();
#if WITH_SENDBIRD
	TWeakObjectPtr<USBBlueprintAsyncCompletion> WeakAsyncCompletion = AsyncCompletion;

	UWorld* World = WorldContextObject->GetWorld();
	SBDGroupChannel* SelectedChannel = SendbirdChatMananger::Get().GetSelectedGroupChannel(Index, Name);
	if (!ensureMsgf(SelectedChannel, TEXT("[USBBlueprintAsyncCompletion::Sendbird_UpdateGroupChannel] GetSelectedGroupChannel(%d,%s) failed!!"), Index, *Name))
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([AsyncCompletion]() {
			AsyncCompletion->OnFail.Broadcast(TEXT("GetSelectedGroupChannel() failed!!"), -1);
		}));
		return AsyncCompletion;
	}

	SelectedChannel->UpdateChannel(TCHAR_TO_WCHAR(*NewName), false, SelectedChannel->cover_url, SelectedChannel->data, SelectedChannel->custom_type,
		[SelectedChannel, Index, WeakAsyncCompletion, &ChannelInfo](SBDGroupChannel* GroupChannel, SBDError* Error) {
		ProcessCompletionHandler(WeakAsyncCompletion, Error, [Index, GroupChannel, &ChannelInfo]() {
			SendbirdChatMananger::Get().SetCurrentChannel(GroupChannel);
			ChannelInfo = FSBChannelInfo(GroupChannel);
			if (ensure(SendbirdChatMananger::Get().GetGroupChannels().IsValidIndex(Index)))
				SendbirdChatMananger::Get().GetGroupChannels()[Index] = GroupChannel;
		});
	});
#endif
	return AsyncCompletion;
}

USBBlueprintAsyncCompletion* USBBlueprintAsyncCompletion::Sendbird_DeleteGroupChannel(UObject* WorldContextObject, int Index, const FString& Name)
{
	USBBlueprintAsyncCompletion* AsyncCompletion = NewObject<USBBlueprintAsyncCompletion>();
#if WITH_SENDBIRD
	TWeakObjectPtr<USBBlueprintAsyncCompletion> WeakAsyncCompletion = AsyncCompletion;

	UWorld* World = WorldContextObject->GetWorld();
	SBDGroupChannel* SelectedChannel = SendbirdChatMananger::Get().GetSelectedGroupChannel(Index, Name);
	if (!ensureMsgf(SelectedChannel, TEXT("[USBBlueprintAsyncCompletion::Sendbird_DeleteGroupChannel] GetSelectedGroupChannel(%d,%s) failed!!"), Index, *Name))
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([AsyncCompletion]() {
			AsyncCompletion->OnFail.Broadcast(TEXT("GetSelectedGroupChannel() failed!!"), -1);
		}));
		return AsyncCompletion;
	}

	for (SBDMember& Member : SelectedChannel->members)
	{
		if (Member.user_id == SendbirdChatMananger::Get().GetCurrentUser()->user_id)
		{
			SelectedChannel->LeaveChannel([Index, WeakAsyncCompletion](SBDError* Error) {
				ProcessCompletionHandler(WeakAsyncCompletion, Error, [Index]() {
					SendbirdChatMananger::Get().ResetCurrentChannel();
					SendbirdChatMananger::Get().GetGroupChannels().RemoveAt(Index);
				});
			});
			return AsyncCompletion;
		}
	}

	SelectedChannel->DeleteChannel([SelectedChannel, Index, WeakAsyncCompletion](SBDError* Error) {
		ProcessCompletionHandler(WeakAsyncCompletion, Error, [Index]() {
			SendbirdChatMananger::Get().GetGroupChannels().RemoveAt(Index);
		});
	});
#endif
	return AsyncCompletion;
}

USBBlueprintAsyncCompletion* USBBlueprintAsyncCompletion::Sendbird_GetGroupChannelList(UObject* WorldContextObject, bool bClearList, TArray<struct FSBChannelInfo>& GroupChannelInfos)
{
	USBBlueprintAsyncCompletion* AsyncCompletion = NewObject<USBBlueprintAsyncCompletion>();
#if WITH_SENDBIRD
	UWorld* MyWorld = WorldContextObject->GetWorld();
	if (bClearList)
	{
		SBDGroupChannelListQuery* GroupChannelListQuery = SendbirdChatMananger::Get().CreateGroupChannelListQuery();
		if (!ensureMsgf(GroupChannelListQuery, TEXT("[USBBlueprintAsyncCompletion::Sendbird_GetGroupChannelList] CreateGroupChannelListQuery() failed!!")))
		{
			MyWorld->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([AsyncCompletion]() {
				AsyncCompletion->OnFail.Broadcast(TEXT("CreateGroupChannelListQuery() failed!!"), -1);
			}));
			return AsyncCompletion;
		}
		SendbirdChatMananger::Get().ResetGroupChannels();
	}

	SBDGroupChannelListQuery* GroupChannelListQuery = SendbirdChatMananger::Get().GetGroupChannelListQuery();
	if (!ensureMsgf(GroupChannelListQuery, TEXT("[USBBlueprintAsyncCompletion::Sendbird_GetGroupChannelList] GetGroupChannelListQuery() failed!!")))
	{
		MyWorld->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([AsyncCompletion]() {
			AsyncCompletion->OnFail.Broadcast(TEXT("GetGroupChannelListQuery() failed!!"), -1);
		}));
		return AsyncCompletion;
	}

	if (!GroupChannelListQuery->has_next)
	{
		MyWorld->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([AsyncCompletion]() {
			AsyncCompletion->OnSuccess.Broadcast(FString(TEXT("")), 0);
		}));
		return AsyncCompletion;
	}

	TWeakObjectPtr<USBBlueprintAsyncCompletion> WeakAsyncCompletion = AsyncCompletion;
	GroupChannelListQuery->LoadNextPage([&GroupChannelInfos, WeakAsyncCompletion](std::vector<SBDGroupChannel*> GroupChannels, SBDError* Error) {
		ProcessCompletionHandler(WeakAsyncCompletion, Error, [&GroupChannelInfos, &GroupChannels]() {
			GroupChannelInfos.Empty();
			for (SBDGroupChannel* GroupChannel : GroupChannels)
			{
				SendbirdChatMananger::Get().GetGroupChannels().Add(GroupChannel);
				GroupChannelInfos.Add(FSBChannelInfo(GroupChannel));
			}
		});
	});
#endif
	return AsyncCompletion;
}

USBBlueprintAsyncCompletion* USBBlueprintAsyncCompletion::Sendbird_JoinChannel(UObject* WorldContextObject, int Index, const FString& Name, TArray<FSBUserInfo>& Members)
{
	USBBlueprintAsyncCompletion* AsyncCompletion = NewObject<USBBlueprintAsyncCompletion>();
#if WITH_SENDBIRD
	TWeakObjectPtr<USBBlueprintAsyncCompletion> WeakAsyncCompletion = AsyncCompletion;

	UWorld* World = WorldContextObject->GetWorld();
	SBDGroupChannel* SelectedChannel = SendbirdChatMananger::Get().GetSelectedGroupChannel(Index, Name);
	if (!ensureMsgf(SelectedChannel, TEXT("[USBBlueprintAsyncCompletion::Sendbird_JoinChannel] GetSelectedGroupChannel(%d,%s) failed!!"), Index, *Name))
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([AsyncCompletion]() {
			AsyncCompletion->OnFail.Broadcast(TEXT("GetSelectedGroupChannel() failed!!"), -1);
		}));
		return AsyncCompletion;
	}

	bool bAlreadyInclude = false;
	for (SBDMember& Member : SelectedChannel->members)
	{
		SendbirdChatMananger::Get().ResetUserList();
		const FSBUserInfo UesrInfo(Member);
		SendbirdChatMananger::Get().GetUserList().Add(Member);
		Members.Add(UesrInfo);
		if (Member.user_id == SendbirdChatMananger::Get().GetCurrentUser()->user_id)
			bAlreadyInclude = true;
	}

	if (bAlreadyInclude)
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([SelectedChannel, AsyncCompletion]() {
			SendbirdChatMananger::Get().SetCurrentChannel(SelectedChannel);
			AsyncCompletion->OnSuccess.Broadcast(TEXT(""), 0);
		}));
		return AsyncCompletion;
	}

	SelectedChannel->JoinChannel([SelectedChannel, Name, WeakAsyncCompletion, &Members](SBDError* Error) {
		ProcessCompletionHandler(WeakAsyncCompletion, Error, [SelectedChannel, &Members]() {
			FSBUserInfo UesrInfo(SendbirdChatMananger::Get().GetCurrentUser());
			Members.Add(UesrInfo);
			SendbirdChatMananger::Get().GetUserList().Add(*SendbirdChatMananger::Get().GetCurrentUser());
			SendbirdChatMananger::Get().SetCurrentChannel(SelectedChannel);
		});

	});
#endif
	return AsyncCompletion;
}

USBBlueprintAsyncCompletion* USBBlueprintAsyncCompletion::Sendbird_LeaveChannel(UObject* WorldContextObject)
{
	USBBlueprintAsyncCompletion* AsyncCompletion = NewObject<USBBlueprintAsyncCompletion>();
#if WITH_SENDBIRD
	TWeakObjectPtr<USBBlueprintAsyncCompletion> WeakAsyncCompletion = AsyncCompletion;

	UWorld* World = WorldContextObject->GetWorld();
	if (!ensure(SendbirdChatMananger::Get().GetCurrentChannel()))
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([AsyncCompletion]() {
			AsyncCompletion->OnFail.Broadcast(TEXT("CurrentChannel is null."), -1);
		}));
		return AsyncCompletion;
	}

	if (!ensureMsgf(SendbirdChatMananger::Get().GetCurrentChannel()->is_group_channel, TEXT("[USBBlueprintAsyncCompletion::Sendbird_LeaveChannel] CurrentChannel is not group.!!")))
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([AsyncCompletion]() {
			AsyncCompletion->OnFail.Broadcast(TEXT("CurrentChannel is not group."), -1);
		}));
		return AsyncCompletion;
	}

	SBDGroupChannel* CurrentChannel = static_cast<SBDGroupChannel*>(SendbirdChatMananger::Get().GetCurrentChannel());
	if (!ensureMsgf(CurrentChannel, TEXT("[USBBlueprintAsyncCompletion::Sendbird_LeaveChannel] Not Exist CurrentChannel!!")))
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([AsyncCompletion]() {
			AsyncCompletion->OnFail.Broadcast(TEXT("Not Exist CurrentChannel!!"), -1);
		}));
		return AsyncCompletion;
	}

	CurrentChannel->LeaveChannel([CurrentChannel, WeakAsyncCompletion](SBDError* Error) {
		ProcessCompletionHandler(WeakAsyncCompletion, Error, [CurrentChannel]() {
			SendbirdChatMananger::Get().ResetCurrentChannel();
			SendbirdChatMananger::Get().GetGroupChannels().Remove(CurrentChannel);
		});
	});
#endif
	return AsyncCompletion;
}

USBBlueprintAsyncCompletion* USBBlueprintAsyncCompletion::Sendbird_SendUserMessage(UObject* WorldContextObject, const FString& SendMessage, FSBMessageInfo& MessageInfo)
{
	USBBlueprintAsyncCompletion* AsyncCompletion = NewObject<USBBlueprintAsyncCompletion>();
#if WITH_SENDBIRD
	TWeakObjectPtr<USBBlueprintAsyncCompletion> WeakAsyncCompletion = AsyncCompletion;

	UWorld* World = WorldContextObject->GetWorld();
	SBDBaseChannel* CurrentChannel = SendbirdChatMananger::Get().GetCurrentChannel();
	if (!ensureMsgf(CurrentChannel, TEXT("[USBBlueprintAsyncCompletion::Sendbird_SendUserMessage] Wrong CurrentChannel!!")))
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([AsyncCompletion]() {
			AsyncCompletion->OnFail.Broadcast(TEXT("Wrong CurrentChannel!!"), -1);
		}));
		return AsyncCompletion;
	}

	SBDUserMessageParams Params;
	Params.SetMessage(TCHAR_TO_WCHAR(*SendMessage));
	CurrentChannel->SendUserMessage(Params, [WeakAsyncCompletion, &MessageInfo](SBDUserMessage* UserMessage, SBDError* Error) {
		ProcessCompletionHandler(WeakAsyncCompletion, Error, [&MessageInfo, &UserMessage]() {
			SBDBaseChannel* CurrentChannel = SendbirdChatMananger::Get().GetCurrentChannel();
			if (CurrentChannel && CurrentChannel->is_group_channel)
			{
				SBDGroupChannel* GroupChannel = static_cast<SBDGroupChannel*>(CurrentChannel);
				GroupChannel->MarkAsRead();
			}
			MessageInfo = SendbirdChatMananger::Get().AddHistoryMessage(UserMessage);
		});
	});
#endif
	return AsyncCompletion;
}

USBBlueprintAsyncCompletion* USBBlueprintAsyncCompletion::Sendbird_UpdateUserMessage(UObject* WorldContextObject, int64 MessageID, const FString& NewMessage, FSBMessageInfo& MessageInfo)
{
	USBBlueprintAsyncCompletion* AsyncCompletion = NewObject<USBBlueprintAsyncCompletion>();
#if WITH_SENDBIRD
	TWeakObjectPtr<USBBlueprintAsyncCompletion> WeakAsyncCompletion = AsyncCompletion;

	UWorld* World = WorldContextObject->GetWorld();
	SBDBaseChannel* CurrentChannel = SendbirdChatMananger::Get().GetCurrentChannel();
	if (!ensureMsgf(CurrentChannel, TEXT("[USBBlueprintAsyncCompletion::Sendbird_UpdateUserMessage] GetCurrentChannel() failed!!")))
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([AsyncCompletion]() {
			AsyncCompletion->OnFail.Broadcast(TEXT("GetCurrentChannel() failed!!"), -1);
		}));
		return AsyncCompletion;
	}

	SBDBaseMessage* Message = SendbirdChatMananger::Get().GetHistoryMessage(MessageID);
	if (!ensureMsgf(Message, TEXT("[USBBlueprintAsyncCompletion::Sendbird_UpdateUserMessage] MessageID(%d) is wrong."), MessageID))
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([AsyncCompletion]() {
			AsyncCompletion->OnFail.Broadcast(TEXT("MessageID is wrong."), -1);
		}));
		return AsyncCompletion;
	}

	SBDUserMessage* UserMessage = static_cast<SBDUserMessage*>(Message);
	CurrentChannel->UpdateUserMessage(UserMessage, TCHAR_TO_WCHAR(*NewMessage), UserMessage->data, UserMessage->custom_type, [&MessageInfo, WeakAsyncCompletion](SBDUserMessage* NewUserMessage, SBDError* Error) {
		ProcessCompletionHandler(WeakAsyncCompletion, Error, [&MessageInfo, NewUserMessage]() {
			MessageInfo = SendbirdChatMananger::Get().UpdateMessage(NewUserMessage);
		});
	});
#endif
	return AsyncCompletion;
}

USBBlueprintAsyncCompletion* USBBlueprintAsyncCompletion::Sendbird_DeleteMessage(UObject* WorldContextObject, int64 MessageID)
{
	USBBlueprintAsyncCompletion* AsyncCompletion = NewObject<USBBlueprintAsyncCompletion>();
#if WITH_SENDBIRD
	TWeakObjectPtr<USBBlueprintAsyncCompletion> WeakAsyncCompletion = AsyncCompletion;

	UWorld* World = WorldContextObject->GetWorld();
	SBDBaseChannel* CurrentChannel = SendbirdChatMananger::Get().GetCurrentChannel();
	if (!ensureMsgf(CurrentChannel, TEXT("[USBBlueprintAsyncCompletion::Sendbird_DeleteMessage] GetCurrentChannel() failed!!")))
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([AsyncCompletion]() {
			AsyncCompletion->OnFail.Broadcast(TEXT("GetCurrentChannel() failed!!"), -1);
		}));
		return AsyncCompletion;
	}

	SBDBaseMessage* Message = SendbirdChatMananger::Get().GetHistoryMessage(MessageID);
	if (!ensureMsgf(Message, TEXT("[USBBlueprintAsyncCompletion::Sendbird_DeleteMessage] MessageID(%d) is wrong."), MessageID))
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([AsyncCompletion]() {
			AsyncCompletion->OnFail.Broadcast(TEXT("MessageID is wrong."), -1);
		}));
		return AsyncCompletion;
	}

	CurrentChannel->DeleteMessage(Message, [MessageID, Message, WeakAsyncCompletion](SBDError* Error) {
		ProcessCompletionHandler(WeakAsyncCompletion, Error, [Message, MessageID]() {
			SendbirdChatMananger::Get().DeleteHistoryMessage(MessageID);
		});
	});
#endif
	return AsyncCompletion;
}

void USBBlueprintAsyncCompletion::GetUserList(UWorld* MyWorld, TWeakObjectPtr<USBBlueprintAsyncCompletion> WeakAsyncCompletion, TArray<FSBUserInfo>& UserList)
{
#if WITH_SENDBIRD
	SBDUserListQuery* UserListQuery = SendbirdChatMananger::Get().GetUserListQuery();

	if (!ensureMsgf(UserListQuery, TEXT("[USBBlueprintAsyncCompletion::GetUserList] GetUserListQuery() failed!!")))
	{
		MyWorld->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([WeakAsyncCompletion]() {
			WeakAsyncCompletion->OnFail.Broadcast(TEXT("GetUserListQuery() failed!!"), -1);
		}));
		return;
	}

	if (!UserListQuery->has_next)
	{
		MyWorld->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([WeakAsyncCompletion]() {
			WeakAsyncCompletion->OnSuccess.Broadcast(FString(TEXT("")), 0);
		}));
		return;
	}

	UserListQuery->LoadNextPage([&UserList, WeakAsyncCompletion](std::vector<SBDUser> Users, SBDError* Error) {
		ProcessCompletionHandler(WeakAsyncCompletion, Error, [&UserList, &Users]() {
			UserList.Empty();
			for (SBDUser& User : Users)
			{
				SendbirdChatMananger::Get().GetUserList().Add(User);
				UserList.Add(FSBUserInfo(User));
			}
		});
	});
#endif
}

void USBBlueprintAsyncCompletion::ProcessCompletionHandler(TWeakObjectPtr<USBBlueprintAsyncCompletion> WeakAsyncCompletion, SBDError* Error, std::function<void()> success_handler)
{
	if (Error != nullptr) {
		FString ErrorMessage = WCHAR_TO_TCHAR(Error->message.c_str());
		int64 ErrorCode = Error->code;
		UE_LOG(SendbirdSample, Error, TEXT("[USBBlueprintAsyncCompletion::CompletionHandler] ErrorMessage(%s) ErrorCode(%d)"), *ErrorMessage, ErrorCode);

		AsyncTask(ENamedThreads::GameThread, [WeakAsyncCompletion, ErrorMessage, ErrorCode]() {
			if (WeakAsyncCompletion.IsValid())
				WeakAsyncCompletion->OnFail.Broadcast(ErrorMessage, ErrorCode);
		});
	}
	else
	{
		success_handler();

		AsyncTask(ENamedThreads::GameThread, [WeakAsyncCompletion]() {
			if (WeakAsyncCompletion.IsValid())
				WeakAsyncCompletion->OnSuccess.Broadcast(FString(TEXT("")), 0);
		});
	}
}
