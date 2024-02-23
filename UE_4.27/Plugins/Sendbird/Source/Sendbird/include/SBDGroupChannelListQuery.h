// Copyright (c) 2021 Sendbird, Inc. All rights reserved.

#ifndef SENDBIRD_SBDGROUPCHANNELLISTQUERY_H_
#define SENDBIRD_SBDGROUPCHANNELLISTQUERY_H_

#include "SBDGroupChannel.h"
#include "SBDUser.h"

class FSBDGroupChannelListQuery;

/**
 * The `SBDGroupChannelListQuery` class is a query class for getting the list of group channels.
 * The instance of this class is created by [`CreateMyGroupChannelListQuery()`]() in `SBDGroupChannel` class.
 */
class SBDGroupChannelListQuery final {
public:
	/**
	 * Sets the number of channels per page.
	 */
	int64_t limit;

	/**
	 * If the value is true, the channel list includes empty channel.
	 */
	bool include_empty_channel;

	/**
	 * If the value is true, the channel object of the list includes members list.
	 */
	bool include_member_list;

	/**
	 * Sets the order of the list. The order is defined in `SBDGroupChannelListOrder`.
	 */
	SBDGroupChannelListOrder order;

	/**
	 * Shows if there is a next page
	 */
	bool has_next;

	/**
	 * Sets query type for `include_member_list`.
	 */
	SBDGroupChannelListQueryType query_type;

	/**
	 * Shows if the query is loading.
	 */
	bool is_loading;

	/**
	 * Sets to filter channels by the public state.
	 * The default value is SBDChannelPublicStateFilter::All.
	 *
	 * @since 3.0.3
	 */
	SBDChannelPublicStateFilter channel_public_state_filter;

	/**
	 * Sets to filter channels by the hidden state.
	 * The default value is SBDChannelHiddenStateFilterUnhiddenOnly.
	 */
	SBDChannelHiddenStateFilter channel_hidden_state_filter;

	/**
	 * Sets the custom type filter.
	 *
	 * @param custom_type The custom type to search.
	 */
	void SetCustomTypeFilter(const std::wstring& custom_type);

	/**
	 * Sets the member state filter.
	 *
	 * @param member_state_filter The member state to search.
	 */
	void SetMemberStateFilter(SBDMemberStateFilter member_state_filter);

	/**
	 * Sets <code>SBDGroupChannel</code> URLs filter.
	 * <code>SBDGroupChannel</code> list containing only and exactly the passed <code>SBDGroupChannel</code> URLs will be returned.
	 * This does not cooperate with other filters.
	 *
	 * @param channel_urls The channel urls to search.
	 */
	void SetChannelUrlsFilter(const std::vector<std::wstring>& channel_urls);

	/**
	 * Sets a filter to return only channels that contains the specified group channel name.
	 *
	 * @param channel_name The channel name to search. The query will return the channels include `channel_name`.
	 */
	void SetChannelNameContainsFilter(const std::wstring& channel_name);

	/**
	 * Sets the filter with nickname.
	 * The group channels which have the member that has nickname are returned by `loadNextPageWithCompletionHandler:`(LIKE search).
	 *
	 * @param nickname Nickname to search.
	 */
	void SetNicknameContainsFilter(const std::wstring& nickname);

	/**
	 * Sets the filter with users. The group channels which have the members that contain users are returned by `LoadNextPage()`.
	 * If you pass `SBDGroupChannelListQueryType::And` to `query_type` and A, B to `users`, the channels whose members containing A and B will be returned.
	 * If `SBDGroupChannelListQueryType::Or` is set, the members of the queried channels will be A or B.
	 *
	 * @param users      Users to search.
	 * @param query_type Logical condition applied to filter.
	 */
	void SetUsersIncludeFilter(const std::vector<SBDUser>& users, SBDGroupChannelListQueryType query_type);

	/**
	 * Sets the filter with users. The group channels which have the members that have users are returned by `LoadNextPage()`.
	 * The channels have the `users` members only.
	 *
	 * @param users Users to search.
	 */
	void SetUsersExactFilter(const std::vector<SBDUser>& users);

	/**
	 * Sets to filter channels by the public state. The default value is SBDChannelPublicStateFilter::All.
	 *
	 * @param channel_public_state_filter
	 * @since 3.0.3
	 */
	void SetChannelPublicStateFilter(const SBDChannelPublicStateFilter channel_public_state_filter);

	/**
	 * Sets to filter channels by the hidden state. The default value is SBDChannelHiddenStateFilterUnhiddenOnly.
	 *
	 * @param channel_hidden_state_filter
	 */
	void SetChannelHiddenStateFilter(const SBDChannelHiddenStateFilter channel_hidden_state_filter);

	/**
	 * Gets the list of channels. If this method is repeatedly called, it will retrieve the following pages of the channel list.
	 *
	 * @param completion_handler The handler block to execute. The `channels` is the array of `SBDGroupChannel` instances.
	 */
	void LoadNextPage(std::function<void(std::vector<SBDGroupChannel*> channels, SBDError*)> completion_handler);

private:
	std::string token;
	std::vector<SBDUser> users_filter_exact_match;
	std::vector<SBDUser> users_filter_like_match;
	std::wstring nickname_contains_filter;
	SBDMemberStateFilter member_state_filter;
	std::wstring custom_type_filter;
	std::vector<std::wstring> channel_urls_filter;
	std::wstring channel_name_filter;

	SBDGroupChannelListQuery();

	friend FSBDGroupChannelListQuery;
};

#endif /* SENDBIRD_SBDGROUPCHANNELLISTQUERY_H_ */
