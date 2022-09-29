// Copyright (c) 2021 Sendbird, Inc. All rights reserved.

#ifndef SENDBIRD_SBDGROUPCHANNEL_H_
#define SENDBIRD_SBDGROUPCHANNEL_H_

#include "SBDBaseChannel.h"
#include "SBDBaseMessage.h"
#include "SBDUser.h"
#include "SBDError.h"
#include "SBDReadStatus.h"
#include "SBDMember.h"
#include "SBDGroupChannelListQuery.h"
#include "SBDGroupChannelParams.h"

class SBDUser;
class SBDGroupChannel;
class SBDGroupChannelListQuery;
class FSBDGroupChannel;

/**
 * The `SBDGroupChannel` class represents a group channel which is a private chat.
 * The user who wants to join the group channel has to be invited by another user who is already joined the channel.
 * This class is derived from `SBDBaseChannel`.
 * If the `SBDChannelHandler` is added, the user will automatically receive all messages from the group channels where the user belongs after connection.
 * The `SBDGroupChannel` provides the features of general messaging apps.
 *
 * * Typing indicator.
 * * Read status for each message.
 * * Unread message count in the channel.
 * * Lastest message in the channel.
 *
 * The `SBDGroupChannel` has a special property, `is_distinct`.
 * The distinct property enabled group channel is always reuesd for same channel <span>members</span>.
 * If a new member gets invited or the member left from the channel, then the distinct property disabled automatically.
 * If you don't set distinct property on the channel, it always create new channel for the same <span>members</span>.
 *
 * For more information, see [Group Channel](https://docs.sendbird.com/ios#group_channel).
 */
class SBDGroupChannel final : public SBDBaseChannel {
public:
	/**
	 * The Last message's ID of the channel.
	 */
	int64_t last_message_id;

	/**
	 * Represents the channel is distinct or not.
	 */
	bool is_distinct;

	/**
	 * Represents the channel is public or not.
	 *
	 * @since 3.0.3
	 */
	bool is_public;

	/**
	 * Unread message count of the channel.
	 */
	uint64_t unread_message_count;

	/**
	 * Channel <span>members</span>.
	 */
	std::vector<SBDMember> members;

	/**
	 * The number of <span>members</span>.
	 */
	uint64_t member_count;

	/**
	 * The inviter.
	 */
	SBDUser inviter;

	/**
	 * If there is a inviter, true.
	 */
	bool has_inviter;

	/**
	 * Checks this channel is hidden
	 */
	bool is_hidden;

	/**
	 * The hidden state
	 */
	SBDGroupChannelHiddenState hidden_state;

	/**
	 * Create a group channel with user IDs. The group channel is distinct.
	 *
	 * @param user_ids           The user IDs to be <span>members</span> of the channel.
	 * @param name               The name of group channel.
	 * @param is_distinct        If true, the channel which has the same users is returned.
	 * @param cover_url          The cover image url of group channel.
	 * @param data               The custom data of group channel.
	 * @param custom_type        The custom type of group channel.
	 * @param completion_handler The handler interface to execute.
	 */
	static void CreateChannel(std::vector<std::wstring> user_ids, const std::wstring& name, bool is_distinct, const std::wstring& cover_url,
		const std::wstring& data, const std::wstring& custom_type, std::function<void(SBDGroupChannel*, SBDError*)> completion_handler);

	/**
	 * Create a group channel with user IDs with cover image file. The group channel is distinct.
	 *
	 * @param user_ids              The user IDs to be <span>members</span> of the channel.
	 * @param name                  The name of group channel.
	 * @param is_distinct           If true, the channel which has the same users is returned.
	 * @param cover_image_file_path The cover image file path of group channel.
	 * @param file_mime_type	    The file mime type.
	 * @param data                  The custom data of group channel.
	 * @param custom_type           The custom type of group channel.
	 * @param completion_handler    The handler interface to execute.
	 */
	static void CreateChannel(const std::vector<std::wstring> user_ids, const std::wstring& name, bool is_distinct, const std::wstring& cover_image_file_path,
		const std::wstring& file_mime_type, const std::wstring& data, const std::wstring& custom_type,
		std::function<void(SBDGroupChannel*, SBDError*)> completion_handler);

	/**
	 * Create a group channel with GroupChannelParams.
	 *
	 * @param params             GroupChannelParams.
	 * @param completion_handler The handler interface to execute.
	 * @since 3.0.3
	 */
	static void CreateChannel(SBDGroupChannelParams& params, std::function<void(SBDGroupChannel*, SBDError*)> completion_handler);

	/**
	 * Updates a group channel.
	 *
	 * @param name               The name of group channel.
	 * @param is_distinct        If true, the channel which has the same users is returned.
	 * @param cover_url          The cover image url of group channel.
	 * @param data               The custom data of group channel.
	 * @param custom_type        The custom type of group channel.
	 * @param completion_handler The handler interface to execute.
	 */
	void UpdateChannel(const std::wstring& name, bool is_distinct, const std::wstring& cover_url, const std::wstring& data, const std::wstring& custom_type,
		std::function<void(SBDGroupChannel*, SBDError*)> completion_handler);

	/**
	 * Updates a group channel with file path.
	 *
	 * @param name                  The name of group channel.
	 * @param is_distinct           If true, the channel which has the same users is returned.
	 * @param cover_image_file_path The cover image file path of group channel.
	 * @param file_mime_type	    The file mime type.
	 * @param data                  The custom data of group channel.
	 * @param custom_type           The custom type of group channel.
	 * @param completion_handler    The handler interface to execute.
	 */
	void UpdateChannel(const std::wstring& name, bool is_distinct, const std::wstring& cover_image_file_path, const std::wstring& file_mime_type,
		const std::wstring& data, const std::wstring& custom_type, std::function<void(SBDGroupChannel*, SBDError*)> completion_handler);

	/**
     * Deletes this <code>GroupChannel</code>. Note that only operators of a channel are able to delete it or else, an error will be returned to the handler.
     *
     * @param handler The callback handler.
     * @since 3.0.37
     */
    void DeleteChannel(std::function<void(SBDError*)> completion_handler);

	/**
	 * Refreshes this channel instance.
	 *
	 * @param completion_handler The handler interface to execute.
	 */
	void RefreshChannel(std::function<void(SBDError*)> completion_handler);

	/**
	 * Creates a query for my group channel list.
	 *
	 * @return SBDGroupChannelListQuery instance for the current user.
	 */
	static SBDGroupChannelListQuery* CreateMyGroupChannelListQuery();

	/**
	 * Gets a group channel instance with channel URL from server asynchronously.
	 *
	 * @param channel_url        The channel URL.
	 * @param completion_handler The handler interface to execute.
	 */
	static void GetChannelFromServer(const std::wstring& channel_url, std::function<void(SBDGroupChannel*, SBDError*)> completion_handler);

	/**
	 * Gets a group channel instance from channel URL asynchronously.
	 *
	 * @param channel_url        The channel URL.
	 * @param completion_handler The handler interface to execute.
	 */
	static void GetChannel(const std::wstring& channel_url, std::function<void(SBDGroupChannel*, SBDError*)> completion_handler);

	/**
	 * Invites multiple users to the group channel.
	 *
	 * @param users              The users to be invited.
	 * @param completion_handler The handler interface to execute.
	 */
	void InviteUsers(const std::vector<SBDUser>& users, std::function<void(SBDError*)> completion_handler);

	/**
	 * Hides the group channel. The channel will be hid from the channel list, but it will be appeared again when the other user send a message in the channel.
	 *
	 * @param hide_prev_messages The option to hide the previous message of this channel when the channel will be appeared again.
	 * @param completion_handler The handler interface to execute.
	 */
	void HideChannel(bool hide_prev_messages, std::function<void(SBDError*)> completion_handler);

	/**
	 * Hides the group channel with the auto unhide option. The channel will be hid from the channel list.
	 * If the allowAutoUnhide is YES, the channel will be appeared again when the other user send a message in the channel.
	 * However, if the allowAutoUnhide is NO, the channel will be appeared by UnhideChannel method.
	 */
	void HideChannel(bool hide_prev_messages, bool allow_auto_unhide, std::function<void(SBDError*)> completion_handler);

	/**
	 * Unhides the group channel.
	 *
	 * @param completion_handler The handler interface to execute.
	 */
	void UnhideChannel(std::function<void(SBDError*)> completion_handler);

	/**
	 * Joins the group channel if this channel is public.
	 *
	 * @since 3.0.3
	 */
	void JoinChannel(std::function<void(SBDError*)> completion_handler);

	/**
	 * Leaves the group channel. The channel will be disappeared from the channel list. If join the channel, the invitation is required.
	 *
	 * @param completion_handler The handler block to execute.
	 */
	void LeaveChannel(std::function<void(SBDError*)> completion_handler);

	/**
	 * Marks as read all group channels of the current user.
	 *
	 * @param completion_handler The handler interface to execute.
	 */
	static void MarkAllAsRead(std::function<void(SBDError*)> completion_handler);

	/**
	 * Sends mark as read.
	 * The other <span>members</span> in the channel will receive an event.
	 * The event will be received in `ReadReceiptUpdated()` of `SBDChannelHandler`.
	 */
	void MarkAsRead();

	/**
	 * Returns how many <span>members</span> haven't been read the given message yet.
	 *
	 * @param message The message.
	 * @return Number of unread member count. Zero if all <span>members</span> read the message.
	 */
	int GetReadReceipt(SBDBaseMessage* message);

	/**
	 * Returns the timestamp of the last seen at the channel by user.
	 *
	 * @param user The user
	 * @return the timestamp of the last seen at.
	 */
	int64_t GetLastSeenAt(const SBDUser& user);

	/**
	 * Returns the <span>members</span> who read the message.
	 *
	 * @param message The message.
	 * @return Members who read the message.
	 */
	std::vector<SBDMember> GetReadMembers(SBDBaseMessage* message);

	/**
	 * Returns the <span>members</span> who didn't read the message.
	 *
	 * @param message The message.
	 * @return Members who don't read the message.
	 */
	std::vector<SBDMember> GetUnreadMembers(SBDBaseMessage* message);

	/**
	 * Returns the read status.
	 * @return The read status. If there's no data, it will be empty.
	 */
	std::map<std::wstring, SBDReadStatus> GetReadStatus();

	/**
	 * Checks if the channel has a member that has `user_id`.
	 *
	 * @param user_id User ID.
	 * @return If true, the channel has the member.
	 */
	bool HasMember(const std::wstring& user_id);

	/**
	 * Gets member in the channel.
	 *
	 * @param user_id User ID.
	 * @param member_exist If the member exists, it is true.
	 * @return `SBDUser` object as a member.
	 */
	SBDMember GetMember(const std::wstring& user_id, bool* member_exist);

	/**
	 * Accpets the invitation.
	 *
	 * @param completion_handler The handler interface to execute.
	 */
	void AcceptInvitation(std::function<void(SBDError*)> completion_handler);

	/**
	 * Declines the invitation.
	 *
	 * @param completion_handler The handler interface to execute.
	 */
	void DeclineInvitation(std::function<void(SBDError*)> completion_handler);

	/**
	 * Resets the history in this channel.
	 *
	 * @param completion_handler The handler block to execute.
	 */
	void ResetMyHistory(std::function<void(SBDError*)> completion_handler);

	/**
	 * Gets the group channel count.
	 *
	 * @param member_state_filter The member state of the current user in the channels that are counted.
	 * @param completion_handler The handler interface to execute.
	 */
	static void GetChannelCount(SBDMemberStateFilter member_state_filter,
		std::function<void(uint64_t group_chanel_count, SBDError* error)> completion_handler);

	/**
	 * Gets the last message of this channel. If there is not the last message, nullptr will be returned.
	 *
	 * @return The last message object.
	 */
	const SBDBaseMessage* GetLastMessage();

private:
	int64_t last_start_typing_ts;
	int64_t last_end_typing_ts;
	std::map<std::wstring, SBDMember> member_map;
	static std::mutex member_mutex;
	std::map<std::wstring, int64_t> cached_read_receipt_status;
	std::map<std::wstring, int64_t> cached_typing_status;
	static std::vector<std::unique_ptr<SBDGroupChannelListQuery>> group_channel_list_queries;
	bool has_been_updated;

	SBDGroupChannel(const std::string& dict);
	static SBDGroupChannel* Upsert(SBDGroupChannel* channel);
	void Overwrite(SBDGroupChannel* channel);
	void UpdateTypingStatus(const SBDUser& user, bool start);
	void AddMember(SBDMember user);
	void RemoveMember(SBDMember user);
	void TypingStatusTimeout();
	void UpdateReadReceipt(const std::wstring& user_id, int64_t timestamp);
	void SetChannelHiddenState(SBDGroupChannelHiddenState hidden_state);
	void UpdateMyReadReceipt(int64_t ts);
	static std::map<std::wstring, std::unique_ptr<SBDGroupChannel>> cached_channels;
	static std::mutex cached_channel_mutex;
	static int64_t last_mark_all_as_read_ts;
	static SBDGroupChannel* BuildAndUpsert(std::string dict);
	static void UpdateTypingStatus();
	static void ClearCache();
	static void RemoveChannelFromCache(const std::wstring& channel_url);
	static SBDGroupChannel* GetChannelFromCache(const std::wstring& channel_url);
	static bool HasChannelInCache(const std::wstring& channel_url);
	static void LocalGetChannel(bool from_local, const std::wstring& channel_url, std::function<void(SBDGroupChannel*, SBDError*)> completion_handler);

	void StartTyping();
	void EndTyping();
	bool IsTyping();
	std::vector<SBDMember> GetTypingMembers();

	friend FSBDGroupChannel;
};

#endif /* SENDBIRD_SBDGROUPCHANNEL_H_ */
