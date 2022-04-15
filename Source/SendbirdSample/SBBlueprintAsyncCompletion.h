// Copyright (c) 2021 Sendbird, Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "SBCommonEnum.h"
#include "SBCommonStruct.h"
#include "SBBlueprintAsyncCompletion.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSBCompletionDelegate, const FString&, Error, int64, Code);

UCLASS()
class USBBlueprintAsyncCompletion : public UBlueprintAsyncActionBase
{
	GENERATED_UCLASS_BODY()

public:
	//+ Common
	UFUNCTION(BlueprintCallable, Category = "Sendbird")
	static FString Sendbird_GetSdkVersion();

	UFUNCTION(BlueprintCallable, Category = "Sendbird")
	static bool Sendbird_Init(const FString& AppID);

	UFUNCTION(BlueprintCallable, Category = "Sendbird")
	static void Sendbird_SetChannelInterface(class UObject* ChannelInterface);

	UFUNCTION(BlueprintCallable, Category = "Sendbird", meta = (BlueprintInternalUseOnly = "true"))
	static class USBBlueprintAsyncCompletion* Sendbird_Connect(const FString& UserID, const FString& AccessToken, FSBUserInfo& LoginUserInfo);
	
	UFUNCTION(BlueprintCallable, Category = "Sendbird", meta = (BlueprintInternalUseOnly = "true"))
	static class USBBlueprintAsyncCompletion* Sendbird_Disconnect();

	UFUNCTION(BlueprintCallable, Category = "Sendbird")
	static void Sendbird_Cleanup();
	//- Common

	//+ User
	UFUNCTION(BlueprintCallable, Category = "Sendbird")
	static void Sendbird_GetCurrentUserInfo(FSBUserInfo& UserInfo, bool& bSuccessed);

	UFUNCTION(BlueprintCallable, Category = "Sendbird", meta = (BlueprintInternalUseOnly = "true"))
	static class USBBlueprintAsyncCompletion* Sendbird_UpdateCurrentUserInfo(const FString& NickName, const FString& ProfileUrl);
		
	UFUNCTION(BlueprintCallable, Category = "Sendbird", meta = (BlueprintInternalUseOnly = "true", bClearList = false, HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", WorldContext = "WorldContextObject"))
	static class USBBlueprintAsyncCompletion* Sendbird_GetFriendList(UObject* WorldContextObject,bool bClearList, TArray<struct FSBUserInfo>& FriendInfos);

	UFUNCTION(BlueprintCallable, Category = "Sendbird", meta = (BlueprintInternalUseOnly = "true", HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", WorldContext = "WorldContextObject"))
	static class USBBlueprintAsyncCompletion* Sendbird_FindUser(UObject* WorldContextObject, const FString& UserID, FSBUserInfo& UserInfo);
	//- User

	//+ Profile
	UFUNCTION(BlueprintCallable, Category = "Sendbird", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", WorldContext = "WorldContextObject"))
	static void Sendbird_RegisterProfileTexture(UObject* WorldContextObject, const FString& ProfileUrl, UTexture2DDynamic* ProfileTexture);

	UFUNCTION(BlueprintCallable, Category = "Sendbird", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", WorldContext = "WorldContextObject"))
	static  UTexture2DDynamic* Sendbird_GetProfileTexture(UObject* WorldContextObject, const FString& ProfileUrl);
	//- Profile

	//+ OpenChannel
	UFUNCTION(BlueprintCallable, Category = "Sendbird", meta = (BlueprintInternalUseOnly = "true"))
	static class USBBlueprintAsyncCompletion* Sendbird_CreateOpenChannel(const FString& Name, FSBChannelInfo& OpenChannelInfo);
	
	UFUNCTION(BlueprintCallable, Category = "Sendbird", meta = (BlueprintInternalUseOnly = "true", HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", WorldContext = "WorldContextObject"))
	static class USBBlueprintAsyncCompletion* Sendbird_UpdateOpenChannel(UObject* WorldContextObject, int Index, const FString& Name, const FString& NewName, FSBChannelInfo& ChannelInfo);
	
	UFUNCTION(BlueprintCallable, Category = "Sendbird", meta = (BlueprintInternalUseOnly = "true", HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", WorldContext = "WorldContextObject"))
	static class USBBlueprintAsyncCompletion* Sendbird_DeleteOpenChannel(UObject* WorldContextObject, int Index, const FString& Name);
	
	UFUNCTION(BlueprintCallable, Category = "Sendbird", meta = (BlueprintInternalUseOnly = "true", bClearList = false, HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", WorldContext = "WorldContextObject"))
	static class USBBlueprintAsyncCompletion* Sendbird_GetOpenChannelList(UObject* WorldContextObject,bool bClearList, TArray<struct FSBChannelInfo>& OpenChannelInfos);
	
	UFUNCTION(BlueprintCallable, Category = "Sendbird", meta = (BlueprintInternalUseOnly = "true", HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", WorldContext = "WorldContextObject"))
	static class USBBlueprintAsyncCompletion* Sendbird_GetOpenChannelParticipantList(UObject* WorldContextObject, bool bClearList, TArray<struct FSBUserInfo>& UserList);

	UFUNCTION(BlueprintCallable, Category = "Sendbird", meta = (BlueprintInternalUseOnly = "true", HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", WorldContext = "WorldContextObject"))
	static class USBBlueprintAsyncCompletion* Sendbird_EnterOpenChannel(UObject* WorldContextObject, int Index, const FString& Name);
	
	UFUNCTION(BlueprintCallable, Category = "Sendbird", meta = (BlueprintInternalUseOnly = "true", HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", WorldContext = "WorldContextObject"))
	static class USBBlueprintAsyncCompletion* Sendbird_ExitOpenChannel(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "Sendbird", meta = (BlueprintInternalUseOnly = "true", HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", WorldContext = "WorldContextObject"))
	static class USBBlueprintAsyncCompletion* TryExitOpenChannel(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "Sendbird", meta = (BlueprintInternalUseOnly = "true", HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", WorldContext = "WorldContextObject"))
	static class USBBlueprintAsyncCompletion* Sendbird_LoadChannelChatHistory(UObject* WorldContextObject, TArray<FSBMessageInfo>& MessageInfos);
	//- OpenChannel

	//+ GroupChannel
	UFUNCTION(BlueprintCallable, Category = "Sendbird", meta = (BlueprintInternalUseOnly = "true"))
	static class USBBlueprintAsyncCompletion* Sendbird_CreateGroupChannel(const FString& Name, FSBChannelInfo& GroupChannelInfo);

	UFUNCTION(BlueprintCallable, Category = "Sendbird", meta = (BlueprintInternalUseOnly = "true", bClearList = false, HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", WorldContext = "WorldContextObject"))
	static class USBBlueprintAsyncCompletion* Sendbird_CreateChannelWithUserIds(UObject* WorldContextObject, const FString& ChannelName, const TArray<FString>& FriendIds);

	UFUNCTION(BlueprintCallable, Category = "Sendbird", meta = (BlueprintInternalUseOnly = "true", HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", WorldContext = "WorldContextObject"))
	static class USBBlueprintAsyncCompletion* Sendbird_UpdateGroupChannel(UObject* WorldContextObject, int Index, const FString& Name, const FString& NewName, FSBChannelInfo& ChannelInfo);

	UFUNCTION(BlueprintCallable, Category = "Sendbird", meta = (BlueprintInternalUseOnly = "true", HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", WorldContext = "WorldContextObject"))
	static class USBBlueprintAsyncCompletion* Sendbird_DeleteGroupChannel(UObject* WorldContextObject, int Index, const FString& Name);

	UFUNCTION(BlueprintCallable, Category = "Sendbird", meta = (BlueprintInternalUseOnly = "true", bClearList = false, HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", WorldContext = "WorldContextObject"))
	static class USBBlueprintAsyncCompletion* Sendbird_GetGroupChannelList(UObject* WorldContextObject,bool bClearList, TArray<struct FSBChannelInfo>& GroupChannelInfos);

	UFUNCTION(BlueprintCallable, Category = "Sendbird", meta = (BlueprintInternalUseOnly = "true", HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", WorldContext = "WorldContextObject"))
	static class USBBlueprintAsyncCompletion* Sendbird_JoinChannel(UObject* WorldContextObject, int Index, const FString& Name, TArray<FSBUserInfo>& Members);

	UFUNCTION(BlueprintCallable, Category = "Sendbird", meta = (BlueprintInternalUseOnly = "true", HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", WorldContext = "WorldContextObject"))
	static class USBBlueprintAsyncCompletion* Sendbird_LeaveChannel(UObject* WorldContextObject);
	//- GroupChannel

	//+ Message
	UFUNCTION(BlueprintCallable, Category = "Sendbird", meta = (BlueprintInternalUseOnly = "true", HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", WorldContext = "WorldContextObject"))
	static class USBBlueprintAsyncCompletion* Sendbird_SendUserMessage(UObject* WorldContextObject, const FString& SendMessage, FSBMessageInfo& MessageInfo);

	UFUNCTION(BlueprintCallable, Category = "Sendbird", meta = (BlueprintInternalUseOnly = "true", HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", WorldContext = "WorldContextObject"))
	static class USBBlueprintAsyncCompletion* Sendbird_UpdateUserMessage(UObject* WorldContextObject, int64 MessageID, const FString& NewMessage, FSBMessageInfo& MessageInfo);

	UFUNCTION(BlueprintCallable, Category = "Sendbird", meta = (BlueprintInternalUseOnly = "true", HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", WorldContext = "WorldContextObject"))
	static class USBBlueprintAsyncCompletion* Sendbird_DeleteMessage(UObject* WorldContextObject, int64 MessageID);
	//- Message
	
private:
	static void GetUserList(UWorld* MyWorld, TWeakObjectPtr<USBBlueprintAsyncCompletion> WeakAsyncCompletion, TArray<struct FSBUserInfo>& UserList);
	static void	ProcessCompletionHandler(TWeakObjectPtr<USBBlueprintAsyncCompletion> WeakAsyncCompletion, SBDError* Error, std::function<void()> success_handler);

public:
	UPROPERTY(BlueprintAssignable)
	FSBCompletionDelegate OnSuccess;

	UPROPERTY(BlueprintAssignable)
	FSBCompletionDelegate OnFail;
};
