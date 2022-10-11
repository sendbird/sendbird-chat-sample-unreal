// Copyright (c) 2021 Sendbird, Inc. All rights reserved.

#pragma once

#include "SBChatCommonEnum.generated.h"

UENUM(BlueprintType)
enum class ESBMessageType : uint8
{
	SBDMessageTypeUser,
	SBDMessageTypeFile,
	SBDMessageTypeAdmin,
};

UENUM(BlueprintType)
enum class ESBChannelUserHandlerType : uint8
{
	UserEntered,
	UserExited,
	UserJoined,
	UserLeft,
};
