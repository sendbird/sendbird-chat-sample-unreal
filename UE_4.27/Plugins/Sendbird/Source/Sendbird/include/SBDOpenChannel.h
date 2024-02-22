// Copyright (c) 2021 Sendbird, Inc. All rights reserved.

#ifndef SENDBIRD_SBDOPENCHANNEL_H_
#define SENDBIRD_SBDOPENCHANNEL_H_

#include "SBDBaseChannel.h"
#include "SBDUser.h"
#include "SBDError.h"
#include "SBDOpenChannelListQuery.h"
#include "SBDUserListQuery.h"
#include "SBDOpenChannelParams.h"

class SBDOpenChannel;
class SBDOpenChannelListQuery;
class FSBDOpenChannel;

/**
 * The `SBDOpenChannel` class represents a public chat.
 * This is a channel type which anyone can participate without a permission. It can handle thousands of users in one channel.
 * This channel has participants instead of members of `SBDGroupChannel` and every participant is always online.
 * A user can be included as an operator when a channel is created. The operator has a permission to mute, unmute, ban and unban participants.
 * The muted user can see the messages in the channel, but can't say anything. The unbanned user can't enter the channel.
 * If a user who is in the channel is banned, the user will be kicked from the channel.
 */
class SBDOpenChannel final : public SBDBaseChannel {
public:
	/**
	 * The number of participants in this channel.
	 */
	int64_t participant_count;

	/**
	 * The operators of this channel.
	 */
	std::vector<SBDUser> operators;

	/**
	 * The frozen state of this channel.
	 */
	bool is_frozen;

	/**
	 * Creates a query instance for open channel list.
	 *
	 * @return The query instance for open channel list.
	 */
	static SBDOpenChannelListQuery* CreateOpenChannelListQuery();

	/**
	 * Updates an open channel with properties.
	 *
	 * @param name               The name of the channel.
	 * @param channel_url        The channel URL. If the length of url is zero, channel url will be determined randomly.
	 * @param cover_url          The cover image URL.
	 * @param data               The data for channel.
	 * @param operator_user_ids  The operator userIds of channel.
	 * @param custom_type        The custom type for channel.
	 * @param completion_handler The handler interface to execute.
	 */
	static void CreateChannel(const std::wstring& name, const std::wstring& channel_url, const std::wstring& cover_url,
		const std::wstring& data, const std::vector<std::wstring>& operator_user_ids, const std::wstring& custom_type,
		std::function<void(SBDOpenChannel*, SBDError*)> completion_handler);

	/**
	 * Updates an open channel with properties.
	 *
	 * @param name                  The name of the channel.
	 * @param channel_url           The channel URL. If the length of url is zero, channel url will be determined randomly.
	 * @param cover_image_file_path The cover image file path.
	 * @param file_mime_type        The file mime type.
	 * @param data                  The data for channel.
	 * @param operator_user_ids     The operator userIds of channel.
	 * @param custom_type           The custom type for channel.
	 * @param completion_handler    The handler interface to execute.
	 */
	static void CreateChannel(const std::wstring& name, const std::wstring& channel_url, const std::wstring& cover_image_file_path,
		const std::wstring& file_mime_type, const std::wstring& data, const std::vector<std::wstring>& operator_user_ids,
		const std::wstring& custom_type, std::function<void(SBDOpenChannel*, SBDError*)> completion_handler);

	/**
	 * Create a open channel with GroupChannelParams.
	 *
	 * @param params             SBDOpenChannelParams.
	 * @param completion_handler The handler interface to execute.
	 * @since 3.0.37
	 */
	static void CreateChannel(SBDOpenChannelParams& params, std::function<void(SBDOpenChannel*, SBDError*)> completion_handler);

	/**
	 * Updates an open channel.
	 *
	 * @param new_name              The name of the channel.
	 * @param new_cover_url         The cover image URL.
	 * @param new_data              The data for channel.
	 * @param new_operator_user_ids The operator user IDs of channel.
	 * @param new_custom_type       The custom type for channel.
	 * @param completion_handler    The handler interface to execute.
	 */
	void UpdateChannel(const std::wstring& new_name, const std::wstring& new_cover_url, const std::wstring& new_data, const std::vector<std::wstring>& new_operator_user_ids,
		const std::wstring& new_custom_type, std::function<void(SBDOpenChannel*, SBDError*)> completion_handler);

	/**
	 * Updates an open channel.
	 *
	 * @param new_name                  The name of the channel.
	 * @param new_cover_image_file_path The cover image file path.
	 * @param new_cover_file_mime_type  The file mime type.
	 * @param new_data                  The data for channel.
	 * @param new_operator_user_ids     The operator user IDs of channel.
	 * @param new_custom_type           The custom type for channel.
	 * @param completion_handler        The handler interface to execute.
	 */
	void UpdateChannel(const std::wstring& new_name, const std::wstring& new_cover_image_file_path, const std::wstring& new_cover_file_mime_type,
		const std::wstring& new_data, const std::vector<std::wstring>& new_operator_user_ids, const std::wstring& new_custom_type,
		std::function<void(SBDOpenChannel*, SBDError*)> completion_handler);

	/**
     * Deletes an <code>OpenChannel</code>. Note that only operators of a channel are able to delete it or else, an error will be returned to the handler.
     *
     * @param handler The callback handler.
	 * @since 3.0.37
     */
    void DeleteChannel(std::function<void(SBDError*)> completion_handler);

	/**
	 * Gets an open channel instance from channel URL.
	 *
	 * @param channel_url        The channel URL.
	 * @param completion_handler The handler interface to execute.
	 */
	static void GetChannel(const std::wstring& channel_url, std::function<void(SBDOpenChannel*, SBDError*)> completion_handler);

	/**
	 * Enters the channel.
	 *
	 * @param completion_handler The handler interface to execute.
	 */
	void Enter(std::function<void(SBDError*)> completion_handler);

	/**
	 * Exits the channel.
	 *
	 * @param completion_handler The handler interface to execute.
	 */
	void Exit(std::function<void(SBDError*)> completion_handler);

	/**
	 * Creates a query instance for getting participant list of the channel instance.
	 *
	 * @return SBDUserListQuery instance for the participant list of the channel instance.
	 */
	SBDUserListQuery* CreateParticipantListQuery();

	/**
	 * Creates a query instance for getting muted user list of the channel instance.
	 *
	 * @return SBDUserListQuery instance for the muted user list.
	 */
	SBDUserListQuery* CreateMutedUserListQuery();

	/**
	 * Creates a query instance for getting banned user list of the channel instance.
	 *
	 * @return SBDUserListQuery instance for the banned user list.
	 */
	SBDUserListQuery* CreateBannedUserListQuery();

	/**
	 * Refreshes the channel information.
	 *
	 * @param completion_handler The handler interface to execute.
	 */
	void RefreshChannel(std::function<void(SBDError*)> completion_handler);

	/**
	 * Bans a user with the user object.
	 *
	 * @param user               The user object.
	 * @param seconds            Duration for ban in seconds.
	 * @param completion_handler The handler interface to execute.
	 */
	void BanUser(const SBDUser& user, int seconds, std::function<void(SBDError*)> completion_handler);

	/**
	 * Unbans a user with the user object.
	 *
	 * @param user               The user object.
	 * @param completion_handler The handler interface to execute.
	 */
	void UnbanUser(const SBDUser& user, std::function<void(SBDError*)> completion_handler);

	/**
	 * Mutes auser with the user object.
	 *
	 * @param user               The user object.
	 * @param completion_handler The handler block to execute.
	 */
	void MuteUser(const SBDUser& user, std::function<void(SBDError*)> completion_handler);

	/**
	 * Unmutes a user with the user object.
	 *
	 * @param user               The user object.
	 * @param completion_handler The handler block to execute.
	 */
	void UnmuteUser(const SBDUser& user, std::function<void(SBDError*)> completion_handler);

	/**
	 * Returns the user is an operator or not.
	 *
	 * @param user The user object.
	 * @return If true, the user is an operator.
	 */
	bool IsOperator(const SBDUser& user);

private:
	std::mutex operators_lock;
	int64_t operators_updated_at;
	static std::vector<std::unique_ptr<SBDOpenChannelListQuery>> open_channel_list_queries;
	static std::vector<std::unique_ptr<SBDUserListQuery>> user_list_queries;
	static std::map<std::wstring, std::shared_ptr<SBDOpenChannel>> cached_channels;
	static std::map<std::wstring, std::shared_ptr<SBDOpenChannel>> entered_channels;
	static std::mutex cached_channels_mutex;

	SBDOpenChannel(const std::string& dict);
	void UpdateOperators(std::vector<SBDUser> operators, int64_t update_ts);
	static SBDOpenChannel* Upsert(SBDOpenChannel* channel);
	void Overwrite(SBDOpenChannel* channel);
	static void ClearCache();
	static void ClearEnteredChannels();
	static void RemoveChannelFromCache(const std::wstring& channel_url);
	static void RemoveChannelFromEnteredChannels(const std::wstring& channel_url);
	static SBDOpenChannel* BuildAndUpsert(const std::string& dict);
	static void LocalGetChannel(bool from_local, const std::wstring& channel_url, std::function<void(SBDOpenChannel*, SBDError*)> completion_handler);

	friend FSBDOpenChannel;
};

#endif /* SENDBIRD_SBDOPENCHANNEL_H_ */
