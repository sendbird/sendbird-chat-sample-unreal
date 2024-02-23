// Copyright (c) 2021 Sendbird, Inc. All rights reserved.

#ifndef SENDBIRD_SBDUSER_H_
#define SENDBIRD_SBDUSER_H_

#include "SBDTypes.h"

#include <string>
#include <map>
#include <functional>

class SBDError;
class FSBDUser;

/**
 * The `SBDUser` class represents a user. The user is identified by the `user_id`, so the `user_id` has to be unique.
 * The `user_id`, `nickname` and `profile_url` are valid in every `SBDUser` instance, however the `connection_status`
 * and `GetLastSeenAt()` is valid in `SBDUser` instance from `SBDUserListQuery`.
 */
class SBDUser {
public:
	/**
	 * User ID. This has to be unique.
	 */
	std::wstring user_id;

	/**
	 * User nickname.
	 */
	std::wstring nickname;

	/**
	 * Profile image url.
	 */
	std::wstring profile_url;

	/**
	 * User connection status. This is defined in `SBDUserConnectionStatus`.
	 */
	SBDUserConnectionStatus connection_status;

	/**
	 * The lastest time when the user became offline.
	 */
	int64_t last_seen_at;

	/**
	 * Meta data.
	 */
	std::map<std::wstring, std::wstring> meta_data;

	/**
	 * Creates the meta <span>data</span> for the current user.
	 *
	 * @param meta_data          The meta <span>data</span> to be set.
	 * @param completion_handler The handler interface to execute. `meta_data` is the meta <span>data</span> which are set on Sendbird server.
	 */
	void CreateMetaData(const std::map<std::wstring, std::wstring>& meta_data,
		std::function<void(const std::map<std::wstring, std::wstring>& meta_data, SBDError* error)> completion_handler);

	/**
	 * Updates the meta <span>data</span> for the current user.
	 *
	 * @param meta_data          The meta <span>data</span> to be updated.
	 * @param completion_handler The handler interface to execute. `meta_data` is the meta <span>data</span> which are updated on Sendbird server.
	 */
	void UpdateMetaData(const std::map<std::wstring, std::wstring>& meta_data,
		std::function<void(const std::map<std::wstring, std::wstring>& meta_data, SBDError* error)> completion_handler);

	/**
	 * Deletes meta <span>data</span> with key for the current user.
	 *
	 * @param key                The key to be deleted.
	 * @param completion_handler The handler interface to execute.
	 */
	void DeleteMetaData(const std::wstring& key, std::function<void(SBDError*)> completion_handler);

	/**
	 * Deletes all meta <span>data</span> for the current user.
	 *
	 * @param completion_handler The handler interface to execute.
	 */
	void DeleteAllMetaData(std::function<void(SBDError*)> completion_handler);

public:
	SBDUser();
	virtual ~SBDUser() {}

protected:
	SBDUser(const std::string& dict);

	friend FSBDUser;
};

#endif /* SENDBIRD_SBDUSER_H_ */
