// Copyright (c) 2021 Sendbird, Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "SBChatCommonEnum.h"
#include "SBChatCommonStruct.h"
#include "SBChat.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSBChatCallback, const FString&, Error, int64, Code);

UCLASS()
class USBChat : public UBlueprintAsyncActionBase
{
	GENERATED_UCLASS_BODY()

public:
	//+ Common
	UFUNCTION(BlueprintCallable, Category = "SBChat")
	static FString GetSdkVersion();

	UFUNCTION(BlueprintCallable, Category = "SBChat")
	static bool Init(const FString& AppID);

	UFUNCTION(BlueprintCallable, Category = "SBChat")
	static void SetChannelEvent(class UObject* InChannelEvent);

	UFUNCTION(BlueprintCallable, Category = "SBChat", meta = (BlueprintInternalUseOnly = "true"))
	static USBChat* Connect(const FString& UserID, const FString& AccessToken);
	
	UFUNCTION(BlueprintCallable, Category = "SBChat", meta = (BlueprintInternalUseOnly = "true"))
	static USBChat* Disconnect();

	UFUNCTION(BlueprintCallable, Category = "SBChat")
	static void Cleanup();
	//- Common

	//+ User
	UFUNCTION(BlueprintCallable, Category = "SBChat")
	static void GetCurrentUserInfo(FSBUserInfo& UserInfo, bool& bSucceeded);

	UFUNCTION(BlueprintCallable, Category = "SBChat", meta = (BlueprintInternalUseOnly = "true"))
	static USBChat* UpdateCurrentUserInfo(const FString& NickName, const FString& ProfileUrl);

	UFUNCTION(BlueprintCallable, Category = "SBChat", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", WorldContext = "WorldContextObject"))
	static void RegisterProfileTexture(UObject* WorldContextObject, const FString& ProfileUrl, UTexture2DDynamic* ProfileTexture);

	UFUNCTION(BlueprintCallable, Category = "SBChat", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", WorldContext = "WorldContextObject"))
	static UTexture2DDynamic* GetProfileTexture(UObject* WorldContextObject, const FString& ProfileUrl);

	UFUNCTION(BlueprintCallable, Category = "SBChat", meta = (BlueprintInternalUseOnly = "true", bClearList = false, HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", WorldContext = "WorldContextObject"))
	static USBChat* GetAllUserList(UObject* WorldContextObject, bool bClearList, TArray<FSBUserInfo>& FriendInfos);

	UFUNCTION(BlueprintCallable, Category = "SBChat", meta = (BlueprintInternalUseOnly = "true", HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", WorldContext = "WorldContextObject"))
	static USBChat* FindUser(UObject* WorldContextObject, const FString& UserID, FSBUserInfo& UserInfo);
	//- User

	//+ OpenChannel
	UFUNCTION(BlueprintCallable, Category = "SBChat", meta = (BlueprintInternalUseOnly = "true"))
	static USBChat* CreateOpenChannel(const FString& Name, FSBChannelInfo& OpenChannelInfo);
	
	UFUNCTION(BlueprintCallable, Category = "SBChat", meta = (BlueprintInternalUseOnly = "true", HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", WorldContext = "WorldContextObject"))
	static USBChat* UpdateOpenChannel(UObject* WorldContextObject, int Index, const FString& Name, const FString& NewName, FSBChannelInfo& ChannelInfo);
	
	UFUNCTION(BlueprintCallable, Category = "SBChat", meta = (BlueprintInternalUseOnly = "true", HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", WorldContext = "WorldContextObject"))
	static USBChat* DeleteOpenChannel(UObject* WorldContextObject, int Index, const FString& Name);
	
	UFUNCTION(BlueprintCallable, Category = "SBChat", meta = (BlueprintInternalUseOnly = "true", bClearList = false, HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", WorldContext = "WorldContextObject"))
	static USBChat* GetOpenChannelList(UObject* WorldContextObject,bool bClearList, TArray<FSBChannelInfo>& OpenChannelInfos);
	
	UFUNCTION(BlueprintCallable, Category = "SBChat", meta = (BlueprintInternalUseOnly = "true", HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", WorldContext = "WorldContextObject"))
	static USBChat* GetOpenChannelParticipantList(UObject* WorldContextObject, bool bClearList, TArray<FSBUserInfo>& UserList);

	UFUNCTION(BlueprintCallable, Category = "SBChat", meta = (BlueprintInternalUseOnly = "true", HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", WorldContext = "WorldContextObject"))
	static USBChat* EnterOpenChannel(UObject* WorldContextObject, int Index, const FString& Name);
	
	UFUNCTION(BlueprintCallable, Category = "SBChat", meta = (BlueprintInternalUseOnly = "true", HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", WorldContext = "WorldContextObject"))
	static USBChat* ExitOpenChannel(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "SBChat", meta = (BlueprintInternalUseOnly = "true", HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", WorldContext = "WorldContextObject"))
	static USBChat* TryToExitOpenChannel(UObject* WorldContextObject);
	//- OpenChannel

	//+ GroupChannel
	UFUNCTION(BlueprintCallable, Category = "SBChat", meta = (BlueprintInternalUseOnly = "true"))
	static USBChat* CreateGroupChannel(const FString& Name, FSBChannelInfo& GroupChannelInfo);

	UFUNCTION(BlueprintCallable, Category = "SBChat", meta = (BlueprintInternalUseOnly = "true", bClearList = false, HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", WorldContext = "WorldContextObject"))
	static USBChat* CreateGroupChannelWithUserIds(UObject* WorldContextObject, const FString& ChannelName, const TArray<FString>& FriendIds);

	UFUNCTION(BlueprintCallable, Category = "SBChat", meta = (BlueprintInternalUseOnly = "true", HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", WorldContext = "WorldContextObject"))
	static USBChat* UpdateGroupChannel(UObject* WorldContextObject, int Index, const FString& Name, const FString& NewName, FSBChannelInfo& ChannelInfo);

	UFUNCTION(BlueprintCallable, Category = "SBChat", meta = (BlueprintInternalUseOnly = "true", HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", WorldContext = "WorldContextObject"))
	static USBChat* DeleteGroupChannel(UObject* WorldContextObject, int Index, const FString& Name);

	UFUNCTION(BlueprintCallable, Category = "SBChat", meta = (BlueprintInternalUseOnly = "true", bClearList = false, HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", WorldContext = "WorldContextObject"))
	static USBChat* GetGroupChannelList(UObject* WorldContextObject,bool bClearList, TArray<FSBChannelInfo>& GroupChannelInfos);

	UFUNCTION(BlueprintCallable, Category = "SBChat", meta = (BlueprintInternalUseOnly = "true", HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", WorldContext = "WorldContextObject"))
	static USBChat* JoinGroupChannel(UObject* WorldContextObject, int Index, const FString& Name, TArray<FSBUserInfo>& Members);

	UFUNCTION(BlueprintCallable, Category = "SBChat", meta = (BlueprintInternalUseOnly = "true", HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", WorldContext = "WorldContextObject"))
	static USBChat* LeaveGroupChannel(UObject* WorldContextObject);
	//- GroupChannel

	//+ Message
	UFUNCTION(BlueprintCallable, Category = "SBChat", meta = (BlueprintInternalUseOnly = "true", HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", WorldContext = "WorldContextObject"))
	static USBChat* SendUserMessage(UObject* WorldContextObject, const FString& SendMessage, FSBMessageInfo& MessageInfo);

	UFUNCTION(BlueprintCallable, Category = "SBChat", meta = (BlueprintInternalUseOnly = "true", HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", WorldContext = "WorldContextObject"))
	static USBChat* UpdateUserMessage(UObject* WorldContextObject, int64 MessageID, const FString& NewMessage, FSBMessageInfo& MessageInfo);

	UFUNCTION(BlueprintCallable, Category = "SBChat", meta = (BlueprintInternalUseOnly = "true", HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", WorldContext = "WorldContextObject"))
	static USBChat* DeleteMessage(UObject* WorldContextObject, int64 MessageID);

	UFUNCTION(BlueprintCallable, Category = "SBChat", meta = (BlueprintInternalUseOnly = "true", HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", WorldContext = "WorldContextObject"))
	static USBChat* GetPreviousMessageList(UObject* WorldContextObject, TArray<FSBMessageInfo>& MessageInfos);
	//- Message
	
private:
	static void GetUserList(UWorld* MyWorld, TWeakObjectPtr<USBChat> WeakSBChat, TArray<FSBUserInfo>& UserList);
	static void	ProcessCompletionHandler(TWeakObjectPtr<USBChat> WeakSBChat, SBDError* Error, std::function<void()> SuccessHandler);

public:
	UPROPERTY(BlueprintAssignable)
	FSBChatCallback OnSuccess;

	UPROPERTY(BlueprintAssignable)
	FSBChatCallback OnFail;
};
