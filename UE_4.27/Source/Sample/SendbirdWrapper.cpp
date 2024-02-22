// Fill out your copyright notice in the Description page of Project Settings.


#include "SendbirdWrapper.h"

#if WITH_SENDBIRD
void ReconnectionHandler::Started() {
	UE_LOG(LogTemp, Warning, TEXT("Reconnection Started()"));

	AsyncTask(ENamedThreads::GameThread, [this]() {
		if (this->sendbird != nullptr && this->sendbird->reconnectionStartedDelegate.IsBound()) {
			UE_LOG(LogTemp, Warning, TEXT("reconnectionStartedDelegate.Broadcast()"));
			this->sendbird->reconnectionStartedDelegate.Broadcast();
		}
	});
}

void ReconnectionHandler::Succeeded() {
	UE_LOG(LogTemp, Warning, TEXT("Reconnection Succeeded()"));

	AsyncTask(ENamedThreads::GameThread, [this]() {
		if (this->sendbird != nullptr && this->sendbird->reconnectionSucceededDelegate.IsBound()) {
			UE_LOG(LogTemp, Warning, TEXT("reconnectionSucceededDelegate.Broadcast()"));
			this->sendbird->reconnectionSucceededDelegate.Broadcast();
		}
	});
}

void ReconnectionHandler::Failed() {
	UE_LOG(LogTemp, Warning, TEXT("Reconnection Failed()"));

	AsyncTask(ENamedThreads::GameThread, [this]() {
		SBDMain::Reconnect(); // Reconnect

		if (this->sendbird != nullptr && this->sendbird->reconnectionFailedDelegate.IsBound()) {
			UE_LOG(LogTemp, Warning, TEXT("reconnectionFailedDelegate.Broadcast()"));
			this->sendbird->reconnectionFailedDelegate.Broadcast();
		}
	});
}

void ChannelHandler::MessageReceived(SBDBaseChannel* channel, SBDBaseMessage* message) {
	UE_LOG(LogTemp, Warning, TEXT("MessageReceived()"));

	if (channel == nullptr || message == nullptr) {
		return;
	}

	AsyncTask(ENamedThreads::GameThread, [this, channel, message]() {
		if (channel->is_group_channel) {
			SBDGroupChannel* groupChannel = static_cast<SBDGroupChannel*>(channel);
			if (message->message_type == SBDMessageType::User) {
				SBDUserMessage* userMessage = static_cast<SBDUserMessage*>(message);

				FString fstrChannelUrl = FString(UTF8_TO_TCHAR(std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(groupChannel->channel_url).c_str()));
				FString fstrMessage = FString(UTF8_TO_TCHAR(std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(userMessage->message).c_str()));
				FString fstrSender = FString(UTF8_TO_TCHAR(std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(userMessage->sender.user_id).c_str()));
				if (this->sendbird != nullptr) {
					this->sendbird->GroupChannelUserMessageReceived(fstrChannelUrl, fstrMessage, fstrSender);
				}
			}
		} else if (channel->is_open_channel) {
			SBDOpenChannel* openChannel = static_cast<SBDOpenChannel*>(channel);
			if (message->message_type == SBDMessageType::User) {
				SBDUserMessage* userMessage = static_cast<SBDUserMessage*>(message);

				FString fstrChannelUrl = FString(UTF8_TO_TCHAR(std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(openChannel->channel_url).c_str()));
				FString fstrMessage = FString(UTF8_TO_TCHAR(std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(userMessage->message).c_str()));
				FString fstrSender = FString(UTF8_TO_TCHAR(std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(userMessage->sender.user_id).c_str()));
				if (this->sendbird != nullptr) {
					this->sendbird->OpenChannelUserMessageReceived(fstrChannelUrl, fstrMessage, fstrSender);
				}
			}
		}
	});
};
#endif


USendbirdWrapper::USendbirdWrapper()
{
	UE_LOG(LogTemp, Warning, TEXT("USendbirdWrapper()"));

#if WITH_SENDBIRD
	channelHandler = MakeShareable(new ChannelHandler(this));
	reconnectionHandler = MakeShareable(new ReconnectionHandler(this));

	AsyncTask(ENamedThreads::GameThread, [this]() {
		SBDMain::Init(L"78CA61A2-F667-488D-A513-0E17C508E9A9");
	});
#endif
}

USendbirdWrapper::~USendbirdWrapper()
{
	UE_LOG(LogTemp, Warning, TEXT("~USendbirdWrapper()"));

#if WITH_SENDBIRD
	AsyncTask(ENamedThreads::GameThread, [this]() {
		SBDMain::RemoveAllChannelHandlers();
		SBDMain::RemoveAllConnectionHandlers();
	});
#endif
}

FString USendbirdWrapper::GetSdkVersion()
{
	FString version;
#if WITH_SENDBIRD
	version = FString(std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(SBDMain::GetSdkVersion()).c_str());
	UE_LOG(LogTemp, Warning, TEXT("GetSdkVersion(): %s"), *version);
#endif
	return version;
}

void USendbirdWrapper::Connect(const FString& userId)
{
	UE_LOG(LogTemp, Warning, TEXT("Connect()"));

#if WITH_SENDBIRD
	if (userId.IsEmpty()) {
		return;
	}

	AsyncTask(ENamedThreads::GameThread, [this, userId]() {
		std::wstring wstrUserId = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(std::string(TCHAR_TO_UTF8(*userId)));

		SBDMain::Connect(wstrUserId, SBD_NULL_WSTRING, [this, userId](SBDUser* user, SBDError* error) {
			bool isConnected = (error == nullptr);
			if (isConnected) {
				std::string strUserId = std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(user->user_id);
				AsyncTask(ENamedThreads::GameThread, [this, isConnected, strUserId]() {
					if (this->connectDelegate.IsBound()) {
						this->connectDelegate.Broadcast(isConnected, FString(UTF8_TO_TCHAR(strUserId.c_str())));
					}
				});
			} else {
				AsyncTask(ENamedThreads::GameThread, [this, isConnected, userId]() {
					if (this->connectDelegate.IsBound()) {
						this->connectDelegate.Broadcast(isConnected, userId);
					}
				});
			}
		});
	});
#endif
}

void USendbirdWrapper::Disconnect()
{
	UE_LOG(LogTemp, Warning, TEXT("Disconnected()"));

#if WITH_SENDBIRD
	AsyncTask(ENamedThreads::GameThread, [this]() {
		SBDMain::RemoveAllConnectionHandlers();
		SBDMain::RemoveAllChannelHandlers();
		SBDMain::Disconnect([]() {});

		if (this->disconnectDelegate.IsBound()) {
			this->disconnectDelegate.Broadcast();
		}
	});
#endif
}

void USendbirdWrapper::AddReconnectionHandler()
{
	UE_LOG(LogTemp, Warning, TEXT("AddReconnectionHandler()"));

#if WITH_SENDBIRD
	AsyncTask(ENamedThreads::GameThread, [this]() {
		SBDMain::RemoveAllConnectionHandlers();
		SBDMain::AddConnectionHandler(L"1", this->reconnectionHandler.Get());
	});
#endif
}

void USendbirdWrapper::AddChannelHandler()
{
	UE_LOG(LogTemp, Warning, TEXT("AddChannelHandler()"));

#if WITH_SENDBIRD
	AsyncTask(ENamedThreads::GameThread, [this]() {
		SBDMain::RemoveAllChannelHandlers();
		SBDMain::AddChannelHandler(L"1", this->channelHandler.Get());
	});
#endif
}

void USendbirdWrapper::GroupChannelUserMessageReceived(const FString& channelUrl, const FString& message, const FString& sender) {
	UE_LOG(LogTemp, Warning, TEXT("GroupChannelUserMessageReceived(%s, %s, %s)"), *channelUrl, *message, *sender);

#if WITH_SENDBIRD
	AsyncTask(ENamedThreads::GameThread, [this, channelUrl, message, sender]() {
		if (this->groupChannelUserMessageReceivedDelegate.IsBound()) {
			UE_LOG(LogTemp, Warning, TEXT("groupChannelUserMessageReceivedDelegate.Broadcast()"));
			this->groupChannelUserMessageReceivedDelegate.Broadcast(channelUrl, message, sender);
		}
	});
#endif
}

void USendbirdWrapper::OpenChannelUserMessageReceived(const FString& channelUrl, const FString& message, const FString& sender) {
	UE_LOG(LogTemp, Warning, TEXT("OpenChannelUserMessageReceived(%s, %s, %s)"), *channelUrl, *message, *sender);

#if WITH_SENDBIRD
	AsyncTask(ENamedThreads::GameThread, [this, channelUrl, message, sender]() {
		if (this->openChannelUserMessageReceivedDelegate.IsBound()) {
			UE_LOG(LogTemp, Warning, TEXT("openChannelUserMessageReceivedDelegate.Broadcast()"));
			this->openChannelUserMessageReceivedDelegate.Broadcast(channelUrl, message, sender);
		}
	});
#endif
}

void USendbirdWrapper::GroupChannelListQuery()
{
	UE_LOG(LogTemp, Warning, TEXT("GroupChannelListQuery()"));

#if WITH_SENDBIRD
	AsyncTask(ENamedThreads::GameThread, [this]() {
		SBDGroupChannelListQuery* query = SBDGroupChannel::CreateMyGroupChannelListQuery();
		query->limit = 20;
		query->LoadNextPage([this](std::vector<SBDGroupChannel*> channels, SBDError* error) {
			bool result = (error == nullptr);
			if (result) {
				AsyncTask(ENamedThreads::GameThread, [this, result, channels]() {
					if (this->groupChannelListQueryDelegate.IsBound()) {
						TArray<FString> channelUrls;
						TArray<FString> channelNames;
						TArray<int> memberCounts;
						for (SBDGroupChannel* channel : channels) {
							channelUrls.Add(FString(UTF8_TO_TCHAR(std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(channel->channel_url).c_str())));
							channelNames.Add(FString(UTF8_TO_TCHAR(std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(channel->name).c_str())));
							memberCounts.Add(static_cast<int>(channel->member_count));
						}
						this->groupChannelListQueryDelegate.Broadcast(result, channelUrls, channelNames, memberCounts);
					}
				});
			} else {
				AsyncTask(ENamedThreads::GameThread, [this, result]() {
					if (this->groupChannelListQueryDelegate.IsBound()) {
						TArray<FString> channelUrls;
						TArray<FString> channelNames;
						TArray<int> memberCounts;
						this->groupChannelListQueryDelegate.Broadcast(result, channelUrls, channelNames, memberCounts);
					}
				});
			}
		});
	});
#endif
}

void USendbirdWrapper::GroupChannelCreate(const FString& channelName, const FString& userIdToInvite)
{
	UE_LOG(LogTemp, Warning, TEXT("GroupChannelCreate()"));

#if WITH_SENDBIRD
	if (channelName.IsEmpty() || userIdToInvite.IsEmpty()) {
		return;
	}

	AsyncTask(ENamedThreads::GameThread, [this, channelName, userIdToInvite]() {
		std::wstring wstrChannelName = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(std::string(TCHAR_TO_UTF8(*channelName)));
		std::wstring wstrUserId = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(std::string(TCHAR_TO_UTF8(*userIdToInvite)));

		std::vector<std::wstring> userIds;
		userIds.push_back(wstrUserId);

		SBDGroupChannel::CreateChannel(SBDGroupChannelParams().SetName(wstrChannelName).SetUserIds(userIds), [this](SBDGroupChannel* channel, SBDError* error) {
			bool result = (error == nullptr);
			AsyncTask(ENamedThreads::GameThread, [this, result]() {
				if (this->groupChannelCreateDelegate.IsBound()) {
					this->groupChannelCreateDelegate.Broadcast(result);
				}
			});
		});
	});
#endif
}

void USendbirdWrapper::GroupChannelMessageListQuery(const FString& channelUrl)
{
	UE_LOG(LogTemp, Warning, TEXT("GroupChannelMessageListQuery()"));

#if WITH_SENDBIRD
	if (channelUrl.IsEmpty()) {
		return;
	}

	AsyncTask(ENamedThreads::GameThread, [this, channelUrl]() {
		std::wstring wstrChannelUrl = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(std::string(TCHAR_TO_UTF8(*channelUrl)));

		SBDGroupChannel::GetChannel(wstrChannelUrl, [this](SBDGroupChannel* channel, SBDError* error) {
			if (error == nullptr) {
				AsyncTask(ENamedThreads::GameThread, [this, channel]() {
					SBDPreviousMessageListQuery* query = channel->CreatePreviousMessageListQuery();
					query->LoadNextPage(20, false, [this](std::vector<SBDBaseMessage*> baseMessages, SBDError* error) {
						if (error == nullptr) {
							AsyncTask(ENamedThreads::GameThread, [this, baseMessages]() {
								if (this->groupChannelMessageListQueryDelegate.IsBound()) {
									TArray<FString> messages;
									TArray<FString> senders;
									for (SBDBaseMessage* baseMessage : baseMessages) {
										SBDUserMessage* userMessage = static_cast<SBDUserMessage*>(baseMessage);
										messages.Add(FString(UTF8_TO_TCHAR(std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(userMessage->message).c_str())));
										senders.Add(FString(UTF8_TO_TCHAR(std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(userMessage->sender.user_id).c_str())));
									}
									this->groupChannelMessageListQueryDelegate.Broadcast(true, messages, senders);
								}
							});
						}
					});
				});
			}
		});
	});
#endif
}

void USendbirdWrapper::GroupChannelSendUserMessage(const FString& channelUrl, const FString& message)
{
	UE_LOG(LogTemp, Warning, TEXT("GroupChannelSendUserMessage()"));

#if WITH_SENDBIRD
	if (channelUrl.IsEmpty() || message.IsEmpty()) {
		return;
	}

	AsyncTask(ENamedThreads::GameThread, [this, channelUrl, message]() {
		std::wstring wstrChannelUrl = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(std::string(TCHAR_TO_UTF8(*channelUrl)));

		SBDGroupChannel::GetChannel(wstrChannelUrl, [this, message](SBDGroupChannel* channel, SBDError* error) {
			if (error == nullptr) {
				AsyncTask(ENamedThreads::GameThread, [this, channel, message]() {
					channel->SendUserMessage(SBDUserMessageParams().SetMessage(TCHAR_TO_WCHAR(*message)), [this](SBDUserMessage* userMessage, SBDError* error) {
						if (error == nullptr) {
							AsyncTask(ENamedThreads::GameThread, [this, userMessage]() {
								if (this->groupChannelSendUserMessageDelegate.IsBound()) {
									FString fstrMessage = FString(UTF8_TO_TCHAR(std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(userMessage->message).c_str()));
									FString fstrSender = FString(UTF8_TO_TCHAR(std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(userMessage->sender.user_id).c_str()));
									this->groupChannelSendUserMessageDelegate.Broadcast(true, fstrMessage, fstrSender);
									UE_LOG(LogTemp, Warning, TEXT("groupChannelSendUserMessageDelegate.Broadcast(): %s"), *fstrMessage);
								}
							});
						}
					});
				});
			}
		});
	});
#endif
}

void USendbirdWrapper::OpenChannelListQuery()
{
	UE_LOG(LogTemp, Warning, TEXT("OpenChannelListQuery()"));

#if WITH_SENDBIRD
	AsyncTask(ENamedThreads::GameThread, [this]() {
		SBDOpenChannelListQuery* query = SBDOpenChannel::CreateOpenChannelListQuery();
		query->limit = 20;
		query->LoadNextPage([this](std::vector<SBDOpenChannel*> channels, SBDError* error) {
			bool result = (error == nullptr);
			if (result) {
				AsyncTask(ENamedThreads::GameThread, [this, result, channels]() {
					if (this->openChannelListQueryDelegate.IsBound()) {
						TArray<FString> channelUrls;
						TArray<FString> channelNames;
						TArray<int> participantCounts;
						for (SBDOpenChannel* channel : channels) {
							channelUrls.Add(FString(UTF8_TO_TCHAR(std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(channel->channel_url).c_str())));
							channelNames.Add(FString(UTF8_TO_TCHAR(std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(channel->name).c_str())));
							participantCounts.Add(static_cast<int>(channel->participant_count));
						}
						this->openChannelListQueryDelegate.Broadcast(result, channelUrls, channelNames, participantCounts);
					}
				});
			} else {
				AsyncTask(ENamedThreads::GameThread, [this, result]() {
					if (this->openChannelListQueryDelegate.IsBound()) {
						TArray<FString> channelUrls;
						TArray<FString> channelNames;
						TArray<int> memberCounts;
						this->openChannelListQueryDelegate.Broadcast(result, channelUrls, channelNames, memberCounts);
					}
				});
			}
		});
	});
#endif
}

void USendbirdWrapper::OpenChannelCreate(const FString& channelName)
{
	UE_LOG(LogTemp, Warning, TEXT("OpenChannelCreate()"));

#if WITH_SENDBIRD
	if (channelName.IsEmpty()) {
		return;
	}

	AsyncTask(ENamedThreads::GameThread, [this, channelName]() {
		std::wstring wstrChannelName = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(std::string(TCHAR_TO_UTF8(*channelName)));

		SBDOpenChannel::CreateChannel(wstrChannelName, SBD_NULL_WSTRING, SBD_NULL_WSTRING, SBD_NULL_WSTRING,
			std::vector<std::wstring>(), SBD_NULL_WSTRING, [this](SBDOpenChannel* channel, SBDError* error) {
			bool result = (error == nullptr);
			AsyncTask(ENamedThreads::GameThread, [this, result]() {
				if (this->openChannelCreateDelegate.IsBound()) {
					this->openChannelCreateDelegate.Broadcast(result);
				}
			});
		});
	});
#endif
}

void USendbirdWrapper::OpenChannelMessageListQuery(const FString& channelUrl)
{
	UE_LOG(LogTemp, Warning, TEXT("OpenChannelMessageListQuery()"));

#if WITH_SENDBIRD
	if (channelUrl.IsEmpty()) {
		return;
	}

	AsyncTask(ENamedThreads::GameThread, [this, channelUrl]() {
		std::wstring wstrChannelUrl = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(std::string(TCHAR_TO_UTF8(*channelUrl)));

		SBDOpenChannel::GetChannel(wstrChannelUrl, [this](SBDOpenChannel* channel, SBDError* error) {
			if (error == nullptr) {
				AsyncTask(ENamedThreads::GameThread, [this, channel]() {
					SBDPreviousMessageListQuery* query = channel->CreatePreviousMessageListQuery();
					query->LoadNextPage(20, false, [this](std::vector<SBDBaseMessage*> baseMessages, SBDError* error) {
						if (error == nullptr) {
							AsyncTask(ENamedThreads::GameThread, [this, baseMessages]() {
								if (this->openChannelMessageListQueryDelegate.IsBound()) {
									TArray<FString> messages;
									TArray<FString> senders;
									for (SBDBaseMessage* baseMessage : baseMessages) {
										SBDUserMessage* userMessage = static_cast<SBDUserMessage*>(baseMessage);
										messages.Add(FString(UTF8_TO_TCHAR(std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(userMessage->message).c_str())));
										senders.Add(FString(UTF8_TO_TCHAR(std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(userMessage->sender.user_id).c_str())));
									}
									this->openChannelMessageListQueryDelegate.Broadcast(true, messages, senders);
								}
							});
						}
					});
				});
			}
		});
	});
#endif
}

void USendbirdWrapper::OpenChannelEnter(const FString& channelUrl)
{
	UE_LOG(LogTemp, Warning, TEXT("OpenChannelEnter()"));

#if WITH_SENDBIRD
	if (channelUrl.IsEmpty()) {
		return;
	}

	AsyncTask(ENamedThreads::GameThread, [this, channelUrl]() {
		std::wstring wstrChannelUrl = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(std::string(TCHAR_TO_UTF8(*channelUrl)));

		SBDOpenChannel::GetChannel(wstrChannelUrl, [this](SBDOpenChannel* channel, SBDError* error) {
			AsyncTask(ENamedThreads::GameThread, [this, channel, error]() {
				if (channel != nullptr && error == nullptr) {
					channel->Enter([this, channel](SBDError* error) {
						if (error == nullptr) {
							AsyncTask(ENamedThreads::GameThread, [this]() {
								if (this->openChannelEnterDelegate.IsBound()) {
									this->openChannelEnterDelegate.Broadcast(true);
									UE_LOG(LogTemp, Warning, TEXT("openChannelEnterDelegate.Broadcast()"));
								}
							});
						} else {
							AsyncTask(ENamedThreads::GameThread, [this]() {
								if (this->openChannelEnterDelegate.IsBound()) {
									this->openChannelEnterDelegate.Broadcast(false);
									UE_LOG(LogTemp, Warning, TEXT("openChannelEnterDelegate.Broadcast()"));
								}
							});
						}
					});
				} else {
					if (this->openChannelEnterDelegate.IsBound()) {
						this->openChannelEnterDelegate.Broadcast(false);
						UE_LOG(LogTemp, Warning, TEXT("openChannelEnterDelegate.Broadcast()"));
					}
				}
			});
		});
	});
#endif
}

void USendbirdWrapper::OpenChannelExit(const FString& channelUrl)
{
	UE_LOG(LogTemp, Warning, TEXT("OpenChannelExit()"));

#if WITH_SENDBIRD
	if (channelUrl.IsEmpty()) {
		return;
	}

	AsyncTask(ENamedThreads::GameThread, [this, channelUrl]() {
		std::wstring wstrChannelUrl = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(std::string(TCHAR_TO_UTF8(*channelUrl)));

		SBDOpenChannel::GetChannel(wstrChannelUrl, [this](SBDOpenChannel* channel, SBDError* error) {
			AsyncTask(ENamedThreads::GameThread, [this, channel, error]() {
				if (channel != nullptr && error == nullptr) {
					channel->Exit([this](SBDError* error) {
						if (error == nullptr) {
							AsyncTask(ENamedThreads::GameThread, [this]() {
								if (this->openChannelExitDelegate.IsBound()) {
									this->openChannelExitDelegate.Broadcast(true);
									UE_LOG(LogTemp, Warning, TEXT("openChannelExitDelegate.Broadcast()"));
								}
							});
						} else {
							AsyncTask(ENamedThreads::GameThread, [this]() {
								if (this->openChannelExitDelegate.IsBound()) {
									this->openChannelExitDelegate.Broadcast(false);
									UE_LOG(LogTemp, Warning, TEXT("openChannelExitDelegate.Broadcast()"));
								}
							});
						}
					});
				} else {
					if (this->openChannelExitDelegate.IsBound()) {
						this->openChannelExitDelegate.Broadcast(false);
						UE_LOG(LogTemp, Warning, TEXT("openChannelExitDelegate.Broadcast()"));
					}
				}
			});
		});
	});
#endif
}

void USendbirdWrapper::OpenChannelSendUserMessage(const FString& channelUrl, const FString& message)
{
	UE_LOG(LogTemp, Warning, TEXT("OpenChannelSendUserMessage()"));

#if WITH_SENDBIRD
	if (channelUrl.IsEmpty() || message.IsEmpty()) {
		return;
	}

	AsyncTask(ENamedThreads::GameThread, [this, channelUrl, message]() {
		std::wstring wstrChannelUrl = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(std::string(TCHAR_TO_UTF8(*channelUrl)));

		SBDOpenChannel::GetChannel(wstrChannelUrl, [this, message](SBDOpenChannel* channel, SBDError* error) {
			AsyncTask(ENamedThreads::GameThread, [this, channel, error, message]() {
				if (channel != nullptr && error == nullptr) {
					channel->SendUserMessage(SBDUserMessageParams().SetMessage(TCHAR_TO_WCHAR(*message)), [this](SBDUserMessage* userMessage, SBDError* error) {
						if (error == nullptr) {
							AsyncTask(ENamedThreads::GameThread, [this, userMessage]() {
								if (this->openChannelSendUserMessageDelegate.IsBound()) {
									FString fstrMessage = FString(UTF8_TO_TCHAR(std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(userMessage->message).c_str()));
									FString fstrSender = FString(UTF8_TO_TCHAR(std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(userMessage->sender.user_id).c_str()));
									this->openChannelSendUserMessageDelegate.Broadcast(true, fstrMessage, fstrSender);
									UE_LOG(LogTemp, Warning, TEXT("openChannelSendUserMessageDelegate.Broadcast(): %s"), *fstrMessage);
								}
							});
						}
					});
				}
			});
		});
	});
#endif
}
