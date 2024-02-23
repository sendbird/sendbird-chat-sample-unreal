// Copyright (c) 2021 Sendbird, Inc. All rights reserved.

#ifndef SENDBIRD_SBDPREVIOUSMESSAGELISTQUERY_H_
#define SENDBIRD_SBDPREVIOUSMESSAGELISTQUERY_H_

#include "SBDBaseChannel.h"

class FSBDPreviousMessageListQuery;

/**
 * An object which retrieves messages from the given channel.
 * The instance of this class is created by [`createPreviousMessageListQuery`]() in `SBDBaseChannel` class.
 */
class SBDPreviousMessageListQuery final {
public:
	/**
	 * Shows if the query is loading. It is true if the query is loading, otherwise returns false.
	 */
	bool is_loading;

	/**
	 * Loads previous message with limit.
	 *
	 * @param limit              The number of messages per page.
	 * @param reverse            If yes, the latest message is the index 0.
	 * @param completion_handler The handler interface to execute.
	 */
	void LoadNextPage(int limit, bool reverse, std::function<void(std::vector<SBDBaseMessage*> messages, SBDError*)> completion_handler);

private:
	SBDBaseChannel* channel;
	int64_t minimum_timestamp;

	SBDPreviousMessageListQuery(SBDBaseChannel* channel);

	friend FSBDPreviousMessageListQuery;
};

#endif /* SENDBIRD_SBDPREVIOUSMESSAGELISTQUERY_H_ */
