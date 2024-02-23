// Copyright (c) 2021 Sendbird, Inc. All rights reserved.

#ifndef SENDBIRD_SBDUSERLISTQUERY_H_
#define SENDBIRD_SBDUSERLISTQUERY_H_

#include "SBDUser.h"
#include "SBDTypes.h"
#include "SBDBaseChannel.h"

class FSBDUserListQuery;

/**
 * SBDUserListQueryType
 */
typedef enum {
	SBDUserListQueryTypeAllUser = 1,
	SBDUserListQueryTypeBlockedUsers = 2,
	SBDUserListQueryTypeOpenChannelParticipants = 3,
	SBDUserListQueryTypeOpenChannelMutedUsers = 4,
	SBDUserListQueryTypeOpenChannelBannedUsers = 5,
	SBDUserListQueryTypeFilteredUsers = 6,
} SBDUserListQueryType;

/**
 * The `SBDUserListQuery` class is a query class for getting the list of all users, participants, blocked users,
 * muted users and banned users. Each type of the query is created by the class or the instance that is related to it.
 *
 * * The query for the all users is created by [`CreateAllUserListQuery()`]() of `SBDMain` class.
 * * The query for the users of the specified user IDs is created by [`CreateUserListQuery()`]() of `SBDMain` class.
 * * The query for the blocked users is created by [`CreateBlockedUserListQuery()`]() of `SBDMain` class.
 * * The query for the participants in the specified open <span>channel</span> is created by [`CreateParticipantListQuery()`]() of the `SBDOpenChannel` instance.
 * * The query for the muted users is created by [`CreateMutedUserListQuery()`]() of the `SBDOpenChannel` instance.
 * * The query for the banned users is created by [`CreateBannedUserListQuery()`]() of the `SBDOpenChannel` instance.
 */
class SBDUserListQuery final {
public:
	/**
	 * The channel instance related to query.
	 */
	SBDBaseChannel* channel;

	/**
	 * Query type. It is defined in `SBDUserListQueryType`.
	 */
	SBDUserListQueryType query_type;

	/**
	 * Sets the number of users per page.
	 */
	int64_t limit;

	/**
	 * Shows if there is a next page
	 */
	bool has_next;

	/**
	 * Shows if the query is loading.
	 */
	bool is_loading;

	/**
	 * Sets meta data filter.
	 *
	 * @param key The key of the meta data to use for filter.
	 * @param values The values of the meta data to use for filter.
	 */
	void SetMetaDataFilter(const std::wstring& key, const std::vector<std::wstring>& values);

	/**
	 * Gets the list of users. If this method is repeatedly called, it will retrieve the following pages of the user list.
	 *
	 * @param completion_handler The handler interface to execute. The `users` is the array of `SBDUser` instances.
	 */
	void LoadNextPage(std::function<void(const std::vector<SBDUser>& users, SBDError* error)> completion_handler);

private:
	std::string token;
	std::wstring meta_data_key;
	std::vector<std::wstring> meta_data_values;
	std::vector<std::wstring> user_ids;

	void LoadAllUserListNextPage(std::function<void(const std::vector<SBDUser>& users, SBDError* error)> completion_handler);
	void LoadBlockedUserListNextPage(std::function<void(const std::vector<SBDUser>& users, SBDError* error)> completion_handler);
	void LoadParticipantListNextPage(std::function<void(const std::vector<SBDUser>& users, SBDError* error)> completion_handler);
	void LoadMutedUserListNextPage(std::function<void(const std::vector<SBDUser>& users, SBDError* error)> completion_handler);
	void LoadBannedUserListNextPage(std::function<void(const std::vector<SBDUser>& users, SBDError* error)> completion_handler);
	void LoadFilteredUserListNextPage(std::function<void(const std::vector<SBDUser>& users, SBDError* error)> completion_handler);

	SBDUserListQuery(SBDUserListQueryType query_type, SBDBaseChannel* channel);
	SBDUserListQuery(const std::vector<std::wstring>& user_ids);

	friend FSBDUserListQuery;
};

#endif /* SENDBIRD_SBDUSERLISTQUERY_H_ */
