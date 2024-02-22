// Copyright (c) 2021 Sendbird, Inc. All rights reserved.

#ifndef SENDBIRD_SBDUSERMESSAGEPARAMS_H_
#define SENDBIRD_SBDUSERMESSAGEPARAMS_H_

#include "SBDTypes.h"

#include <vector>
#include <string>

class SBDCommand;

/**
 * The `SBDUserMessageParams` class is used to send a user message in `SBDBaseChannel`. This is a child class of `SBDBaseMessageParams`.
 */
class SBDUserMessageParams final {
public:
	/**
	 * Constructor
	 */
	SBDUserMessageParams();

	 /**
      * Destructor
      */
    ~SBDUserMessageParams();

	/**
	 * Message text.
	 */
	SBDUserMessageParams& SetMessage(std::wstring message);

	/**
	 * Message data. The default value is null.
	 */
	SBDUserMessageParams& SetData(std::wstring data);

	/**
	 * Customize message's type to filter. The default value is null.
	 */
	SBDUserMessageParams& SetCustomType(std::wstring custom_type);

	/**
	 * The target languages that the message will be translated into.
	 */
	SBDUserMessageParams& SetTargetLanguages(std::vector<std::wstring>& target_languages);

	/**
	 * Set mention type.
	 */
	SBDUserMessageParams& SetMentionType(SBDMentionType mention_type);

	/**
	 * Can mention to specific users. If sends a message with this field. the message will be arrived to mentioned users. The default value is null.
	 */
	SBDUserMessageParams& SetMentionedUserIds(std::vector<std::wstring>& mentioned_user_ids);

	/**
	 * Push option for message. If the option is default, push message will work with the receiver's push settings. If the option is suppress, push message will not be sent.
	 */
	SBDUserMessageParams& SetPushNotificationDeliveryOption(SBDPushNotificationDeliveryOption pushnotification_delivery_option);

	/**
	 * Meta array keys.
	 */
	SBDUserMessageParams& SetMetaArrayKeys(std::vector<std::wstring>& metaarray_keys);

private:
	std::wstring message;
	std::wstring data;
	std::wstring custom_type;
	std::vector<std::wstring> target_languages;
	SBDMentionType mention_type;
	std::vector<std::wstring> mentioned_user_ids;
	SBDPushNotificationDeliveryOption pushnotification_delivery_option;
	std::vector<std::wstring> metaarray_keys;

	friend SBDCommand;
};

#endif /* SENDBIRD_SBDUSERMESSAGEPARAMS_H_ */
