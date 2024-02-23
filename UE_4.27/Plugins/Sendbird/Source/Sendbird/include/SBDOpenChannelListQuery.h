// Copyright (c) 2021 Sendbird, Inc. All rights reserved.

#ifndef SENDBIRD_SBDOPENCHANNELLISTQUERY_H_
#define SENDBIRD_SBDOPENCHANNELLISTQUERY_H_

#include "SBDOpenChannel.h"
#include "SBDError.h"

class FSBDOpenChannelListQuery;

/**
 * The `SBDOpenChannelListQuery` class is a query class for getting the list of open channels.
 * The instance of this class is created by [`CreateOpenChannelListQuery()`]() in `SBDOpenChannel` class.
 */
class SBDOpenChannelListQuery final {
public:
	/**
	 * Sets the number of channels per page.
	 */
	int64_t limit;

	/**
	 * Shows if there is a next page.
	 */
	bool has_next;

	/**
	 * Shows if the query is loading.
	 */
	bool is_loading;

	/**
	 * Sets the channel URL filter.
	 *
	 * @param channel_url The channel url to search.
	 */
	void SetChannelUrlFilter(const std::wstring& channel_url);

	/**
	 * Sets the channel name filter.
	 *
	 * @param channel_name The channel name to search.
	 */
	void SetChannelNameFilter(const std::wstring& channel_name);

	/**
	 * Sets the custom type filter.
	 *
	 * @param custom_type The custom type to search.
	 */
	void SetCustomTypeFilter(const std::wstring& custom_type);

	/**
	 * Gets the list of channels. If this method is repeatedly called, it will retrieve the following pages of the channel list.
	 *
	 * @param completion_handler The handler interface to execute.
	 */
	void LoadNextPage(std::function<void(std::vector<SBDOpenChannel*> channels, SBDError*)> completion_handler);

private:
	std::string token;
	std::wstring channel_url_filter;
	std::wstring channel_name_filter;
	std::wstring custom_type_filter;

	SBDOpenChannelListQuery();

	friend FSBDOpenChannelListQuery;
};

#endif /* SENDBIRD_SBDOPENCHANNELLISTQUERY_H_ */
