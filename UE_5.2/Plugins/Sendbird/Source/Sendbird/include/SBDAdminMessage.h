// Copyright (c) 2021 Sendbird, Inc. All rights reserved.

#ifndef SENDBIRD_SBDADMINMESSAGE_H_
#define SENDBIRD_SBDADMINMESSAGE_H_

#include "SBDBaseMessage.h"

class FSBDAdminMessage;

/**
 * The `SBDAdminMessage` class represents a <span>message</span> which is sent by an admin via [Platform API](https://docs.sendbird.com/platform).
 * The admin <span>message</span> doesn't have a sender.
 * This <span>message</span> is sutable for a notice to inform the members in a group channel or the participants in an open channel.
 * If you would like to know about the Platform API for the <span>message</span>, see [Send a message](https://docs.sendbird.com/platform#messages_3_send).
 */
class SBDAdminMessage final : public SBDBaseMessage {
public:
	/**
	 * Message text.
	 */
	std::wstring message;

	/**
	 * Data of <span>message</span>.
	 */
	std::wstring data;

	/**
	 * Custom message type.
	 */
	std::wstring custom_type;

private:
	SBDAdminMessage(const std::string& dict);
	void Overwrite(SBDAdminMessage* message);

	friend FSBDAdminMessage;
};

#endif /* SENDBIRD_SBDADMINMESSAGE_H_ */
