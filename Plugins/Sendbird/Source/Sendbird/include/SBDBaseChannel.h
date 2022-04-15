// Copyright (c) 2021 Sendbird, Inc. All rights reserved.

#ifndef SENDBIRD_SBDBASECHANNEL_H_
#define SENDBIRD_SBDBASECHANNEL_H_

#include "SBDUser.h"
#include "SBDBaseMessage.h"
#include "SBDUserMessage.h"
#include "SBDAdminMessage.h"
#include "SBDFileMessage.h"
#include "SBDUserMessageParams.h"
#include "SBDFileMessageParams.h"
#include "SBDError.h"
#include "SBDTypes.h"

#include <mutex>
#include <queue>

class SBDPreviousMessageListQuery;
class SBDThumbnailSize;
class SBDThumbnail;
class SBDFileMessage;
class SBDUserMessage;
class SBDGroupChannel;
class SBDOpenChannel;
class SBDOperatorListQuery;
class SBDBaseChannel;
class FSBDBaseChannel;
class SBDFileMessageParams;

/**
 * An object that adopts the `SBDChannelHandler` class is responsible for receiving the events in the channel.
 * Some of virtual methods are common for the `SBDBaseChannel`.
 * However, there are virtual methods for the `SBDOpenChannel` and `SBDGroupChannel` exclusive.
 * The `SBDChannelHandler` can be added by [`void AddChannelHandler(std::wstring identifier, SBDChannelHandler* handler)`]()
 * in `SBDMain`. Every `SBDChannelHandler` method which is added is going to receive events.
 *
 * @warning If the object that adopts the `SBDChannelHandler` class is invalid,
 *          the delegate has to be removed by the identifier via [`void RemoveChannelHandler(string identifier)`]() in `SBDMain`.
 *          If you miss this, it will cause the crash.
 */
class SBDChannelHandler {
public:
	/**
	 * A callback when a message is received.
	 *
	 * @param channel The channel where the message is received.
	 * @param message The received message.
	 */
	virtual void MessageReceived(SBDBaseChannel* channel, SBDBaseMessage* message) {};

	/**
	 * A callback when a message is updated.
	 *
	 * @param channel The channel where the message is updated.
	 * @param message The updated message.
	 */
	virtual void MessageUpdated(SBDBaseChannel* channel, SBDBaseMessage* message) {};

	/**
	 * A callback when read receipts updated.
	 *
	 * @param channel The group channel where the read receipt updated.
	 */
	virtual void ReadReceiptUpdated(SBDGroupChannel* channel) {};

	/**
	 * A callback when users are invited by inviter.
	 *
	 * @param channel The group channel where the invitation is occured.
	 * @param invitees Invitees.
	 * @param inviter Inviter.
	 */
	virtual void InvitationReceived(SBDGroupChannel* channel, const std::vector<SBDUser>& invitees, SBDUser& inviter) {};

	/**
	 * A callback when user declined the invitation.
	 *
	 * @param channel The group channel where the invitation is occured.
	 * @param invitee Invitee.
	 * @param inviter Inviter.
	 */
	virtual void InvitationDeclined(SBDGroupChannel* channel, SBDUser& invitee, SBDUser& inviter) {};

	/**
	 * A callback when new member joined to the group channel.
	 *
	 * @param channel The group channel.
	 * @param user   The new user joined to the channel.
	 */
	virtual void UserJoined(SBDGroupChannel* channel, SBDUser& user) {};

	/**
	 * A callback when current member left from the group channel.
	 *
	 * @param channel The group channel.
	 * @param user   The member left from the channel.
	 */
	virtual void UserLeft(SBDGroupChannel* channel, SBDUser& user) {};

	/**
	 * A callback when a user enter an open channel.
	 *
	 * @param channel The open channel.
	 * @param user   The user
	 */
	virtual void UserEntered(SBDOpenChannel* channel, SBDUser& user) {};

	/**
	 * A callback when a user exit an open channel.
	 *
	 * @param channel The open channel.
	 * @param user   The user.
	 */
	virtual void UserExited(SBDOpenChannel* channel, SBDUser& user) {};

	/**
	 * A callback when a user was muted in the open channel.
	 *
	 * @param channel The open channel.
	 * @param user   The user who was muted.
	 */
	virtual void UserMuted(SBDOpenChannel* channel, SBDUser& user) {};

	/**
	 * A callback when a user was unmuted in the open channel.
	 *
	 * @param channel The open channel.
	 * @param user   The user who was unmuted.
	 */
	virtual void UserUnmuted(SBDOpenChannel* channel, SBDUser& user) {};

	/**
	 * A callback when a user was banned in the open channel.
	 *
	 * @param channel The open channel.
	 * @param user   The user who was banned.
	 */
	virtual void UserBanned(SBDOpenChannel* channel, SBDUser& user) {};

	/**
	 * A callback when a user was unbanned in the open channel.
	 *
	 * @param channel The open channel.
	 * @param user   The user who was unbanned.
	 */
	virtual void UserUnbanned(SBDOpenChannel* channel, SBDUser& user) {};

	/**
	 * A callback when an open channel was frozen.
	 *
	 * @param channel The open channel.
	 */
	virtual void ChannelFrozen(SBDOpenChannel* channel) {};

	/**
	 * A callback when an open channel was unfrozen.
	 *
	 * @param channel The open channel.
	 */
	virtual void ChannelUnfrozen(SBDOpenChannel* channel) {};

	/**
	 * A callback when an open channel was changed.
	 *
	 * @param channel The open channel.
	 */
	virtual void ChannelChanged(SBDBaseChannel* channel) {};

	/**
	 * A callback when an open channel was deleted.
	 *
	 * @param channel_url The open channel.
	 */
	virtual void ChannelDeleted(const std::wstring& channel_url, SBDChannelType channel_type) {};

	/**
	 * A callback when a message was removed in the channel.
	 *
	 * @param channel    The base channel.
	 * @param message_id The message ID which was removed.
	 */
	virtual void MessageDeleted(SBDBaseChannel* channel, uint64_t message_id) {};

	/**
	 * A callback when meta data was created in the channel.
	 *
	 * @param channel The channel that the meta data was created.
	 * @param created_meta_data The created meta data.
	 */
	virtual void ChannelMetaDataCreated(SBDBaseChannel* channel, const std::map<std::wstring, std::wstring>& created_meta_data) {};

	/**
	 * A callback when meta data was updated in the channel.
	 *
	 * @param channel The channel that the meta data was updated.
	 * @param updated_meta_data The updated meta data.
	 */
	virtual void ChannelMetaDataUpdated(SBDBaseChannel* channel, const std::map<std::wstring, std::wstring>& updated_meta_data) {};

	/**
	 * A callback when meta data was deleted in the channel.
	 *
	 * @param channel The channel that the meta data was deleted.
	 * @param deleted_meta_data The keys of the deleted meta data.
	 */
	virtual void ChannelMetaDataDeleted(SBDBaseChannel* channel, const std::vector<std::wstring>& deleted_meta_data) {};

	/**
	 * A callback when meta counters were created in the channel.
	 *
	 * @param channel The channel that the meta counters were created.
	 * @param created_meta_counters The created meta counters.
	 */
	virtual void ChannelMetaCountersCreated(SBDBaseChannel* channel, const std::map<std::wstring, int64_t>& created_meta_counters) {};

	/**
	 * A callback when meta counters were updated in the channel.
	 *
	 * @param channel The channel that the meta counters were updated.
	 * @param updated_meta_counters The updated meta counters.
	 */
	virtual void ChannelMetaCountersUpdated(SBDBaseChannel* channel, const std::map<std::wstring, int64_t>& updated_meta_counters) {};

	/**
	 * A callback when meta counters were deleted in the channel.
	 *
	 * @param channel The channel that the meta counters were deleted.
	 * @param deleted_meta_counters The keys of the deleted meta counters.
	 */
	virtual void ChannelMetaCountersDeleted(SBDBaseChannel* channel, const std::vector<std::wstring>& deleted_meta_counters) {};

	/**
	 * A callback when a mentioned user is received.
	 *
	 * @param channel The channel where the mentioned user is received.
	 * @param message The received message.
	 */
	virtual void MentionReceived(SBDBaseChannel* channel, SBDBaseMessage* message) {};

	/**
	 * A callback when the channel was hidden on the other device or by Platform API.
	 *
	 * @param channel The channel where the channel was hidden.
	 */
	virtual void ChannelWasHidden(SBDGroupChannel* channel) {};

	/**
     * A callback for when operators change in channel.
     *
     * @param channel The <code>BaseChannel</code> the change has occurred.
     * @since 3.0.37
     */
    virtual void OperatorUpdated(SBDBaseChannel* channel) {};
};

/**
 * The `SBDBaseChannel` class represents the channel where users chat each other.
 * The `SBDOpenChannel` and the `SBDGroupChannel` are derived from this class.
 * This class provides the common methods for the `SBDOpenChannel` and the `SBDGroupChannel`.
 *
 * * Send a user message to the channel.
 * * Send a file message to the channel.
 * * Delete a message of the channel.
 * * Create a query for loading messages of the channel.
 * * Manipulate meta counters and meta <span>data</span> of the channel.
 *
 * The channel objects are maintained as a single instance in an application.
 * If you create or get channels from the same channel URL, they must be the same instances.
 */
class SBDBaseChannel {
public:
	/**
	 * The channel URL.
	 */
	std::wstring channel_url;

	/**
	 * The name of channel.
	 */
	std::wstring name;

	/**
	 * The channel cover image URL.
	 */
	std::wstring cover_url;

	/**
	 * The timestamp when the channel is created.
	 */
	int64_t created_at;

	/**
	 * The custom date of the channel.
	 */
	std::wstring data;

	/**
	 * The custom type of the channel.
	 */
	std::wstring custom_type;

	/**
	 * Checks the channel type. If true, this channel is a group channel.
	 */
	bool is_group_channel;

	/**
	 * Checks the channel type. If true, this channel is an open channel.
	 */
	bool is_open_channel;

	/**
	 * Sends a user message with <span>params</span>.
	 *
	 * @param params             The message params
	 * @param completion_handler The handler interface to execute.
	 * @return Returns the temporary user message with a request ID. It doesn't have a message ID.
	 */
	SBDUserMessage* SendUserMessage(SBDUserMessageParams& params, std::function<void(SBDUserMessage*, SBDError*)> completion_handler);

	/**
	 * Sends a file message with binary file data and <span>custom message type</span>.
	 *
	 * @param file_buf           Binary file data.
	 * @param filename           File<span>name</span>.
	 * @param size               File size.
	 * @param type               The type of file.
	 * @param thumbnail_sizes    Thumbnail sizes. This parameter is the std::vector of `SBDThumbnailSize` object and works for image file only.
	 * @param data               Custom <span>data</span>.
	 * @param custom_type        Custom message type.
	 * @param completion_handler The handler interface to execute.
	 * @return Returns the temporary file message with a request ID. It doesn't have a message ID.
	 */
	SBDFileMessage* SendFileMessage(const char* file_buf, const std::wstring& filename, int64_t size, const std::wstring& type,
		std::vector<SBDThumbnailSize> thumbnail_sizes, const std::wstring& data, const std::wstring& custom_type,
		std::function<void(SBDFileMessage*, SBDError*)> completion_handler);

	/**
	 * Sends a file message with SBDFileMessageParams.
	 *
	 * @param params             SBDFileMessageParams.
	 * @param completion_handler The handler interface to execute.
	 * @return Returns the temporary file message with a request ID. It doesn't have a message ID.
	 */
	SBDFileMessage* SendFileMessage(SBDFileMessageParams& params, std::function<void(SBDFileMessage*, SBDError*)> completion_handler);

	/**
	 * Deletes a message. The message's sender has to be the current user.
	 *
	 * @param message            The message to be deleted.
	 * @param completion_handler The handler interface to execute.
	 */
	void DeleteMessage(SBDBaseMessage* message, std::function<void(SBDError*)> completion_handler);

	/**
	 * Updates a user message. The message text, data, and custom type can be updated.
	 *
	 * @param message            The user message to be updated.
	 * @param message_text       New message text.
	 * @param data               New data.
	 * @param custom_type        New custom type.
	 * @param completion_handler The handler interface to execute.
	 */
	void UpdateUserMessage(SBDUserMessage* message, const std::wstring& message_text, const std::wstring& data, const std::wstring& custom_type,
		std::function<void(SBDUserMessage*, SBDError*)> completion_handler);

	/**
	 * Updates a file message. The data and custom type can be updated.
	 *
	 * @param message            The file message to be updated.
	 * @param data               New data.
	 * @param custom_type        New custom type.
	 * @param completion_handler The handler interface to execute.
	 */
	void UpdateFileMessage(SBDFileMessage* message, const std::wstring& data, const std::wstring& custom_type, std::function<void(SBDFileMessage*, SBDError*)> completion_handler);

	/**
	 * Creates `SBDPreviousMessageListQuery` instance for getting the previous messages list of the channel.
	 *
	 * @return Returns the message list of the channel.
	 */
	SBDPreviousMessageListQuery* CreatePreviousMessageListQuery();

	/**
	 * Creates the meta counters for the channel.
	 *
	 * @param meta_counters      The meta counters to be set.
	 * @param completion_handler The handler interface to execute.
	 */
	void CreateMetaCounters(const std::map<std::wstring, int64_t>& meta_counters,
		std::function<void(const std::map<std::wstring, int64_t>& meta_counters, SBDError*)> completion_handler);

	/**
	 * Gets the meta counters with keys for the channel.
	 *
	 * @param keys               The keys to get meta counters.
	 * @param completion_handler The handler interface to execute.
	 */
	void GetMetaCounters(const std::vector<std::wstring>& keys,
		std::function<void(const std::map<std::wstring, int64_t>& meta_counters, SBDError*)> completion_handler);

	/**
	 * Gets all meta counters for the channel.
	 *
	 * @param completion_handler The handler interface to execute.
	 */
	void GetAllMetaCounters(std::function<void(const std::map<std::wstring, int64_t>& meta_counters, SBDError*)> completion_handler);

	/**
	 * Updates the meta counters for the channel.
	 *
	 * @param meta_counters      The meta counters to be updated.
	 * @param completion_handler The handler interface to execute.
	 */
	void UpdateMetaCounters(const std::map<std::wstring, int64_t>& meta_counters,
		std::function<void(const std::map<std::wstring, int64_t>& meta_counters, SBDError*)> completion_handler);

	/**
	 * Increases the meta counters for the channel.
	 *
	 * @param meta_counters      The meta counters to be increased.
	 * @param completion_handler The handler interface to execute.
	 */
	void IncreaseMetaCounters(const std::map<std::wstring, int64_t>& meta_counters,
		std::function<void(const std::map<std::wstring, int64_t>& meta_counters, SBDError*)> completion_handler);

	/**
	 * Decreases the meta counters for the channel.
	 *
	 * @param meta_counters      The meta counters to be decreased.
	 * @param completion_handler The handler interface to execute.
	 */
	void DecreaseMetaCounters(const std::map<std::wstring, int64_t>& meta_counters,
		std::function<void(const std::map<std::wstring, int64_t>& meta_counters, SBDError*)> completion_handler);

	/**
	 * Deletes the meta counters with key for the channel.
	 *
	 * @param key                The key to be deleted.
	 * @param completion_handler The handler interface to execute.
	 */
	void DeleteMetaCounter(const std::wstring& key, std::function<void(SBDError*)> completion_handler);

	/**
	 * Deletes all meta counters for the channel.
	 *
	 * @param completion_handler The handler interface to execute.
	 */
	void DeleteAllMetaCounters(std::function<void(SBDError*)> completion_handler);

	/**
	 * Creates the meta <span>data</span> for the channel.
	 *
	 * @param meta_data          The meta <span>data</span> to be set.
	 * @param completion_handler The handler interface to execute.
	 */
	void CreateMetaData(const std::map<std::wstring, std::wstring>& meta_data,
		std::function<void(const std::map<std::wstring, std::wstring>& meta_data, SBDError*)> completion_handler);

	/**
	 * Gets the meta <span>data</span> for the channel.
	 *
	 * @param keys               The keys to get meta <span>data</span>.
	 * @param completion_handler The handler interface to execute.
	 */
	void GetMetaData(const std::vector<std::wstring>& keys,
		std::function<void(const std::map<std::wstring, std::wstring>& meta_data, SBDError* error)> completion_handler);

	/**
	 * Gets all meta <span>data</span> for the channel.
	 *
	 * @param completion_handler The handler interface to execute.
	 */
	void GetAllMetaData(std::function<void(const std::map<std::wstring, std::wstring>& meta_data, SBDError* error)> completion_handler);

	/**
	 * Updates the meta <span>data</span> for the channel.
	 *
	 * @param meta_data          The meta <span>data</span> to be updated.
	 * @param completion_handler The handler interface to execute.
	 */
	void UpdateMetaData(const std::map<std::wstring, std::wstring>& meta_data,
		std::function<void(const std::map<std::wstring, std::wstring>& meta_data, SBDError*)> completion_handler);

	/**
	 * Deletes meta <span>data</span> with key for the channel.
	 *
	 * @param key                The key to be deleted.
	 * @param completion_handler The handler interface to execute.
	 */
	void DeleteMetaData(const std::wstring& key, std::function<void(SBDError*)> completion_handler);

	/**
	 * Deletes all meta <span>data</span> for the channel.
	 *
	 * @param completion_handler The handler interface to execute.
	 */
	void DeleteAllMetaData(std::function<void(SBDError*)> completion_handler);

	/**
	 * Gets the next messages by the timestamp with a limit and ordering.
	 *
	 * @param timestamp          The standard timestamp to load messages.
	 * @param next_limit         The limit for the number of messages. The returned messages could be more than this number if there are messages which have the same timestamp.
	 * @param reverse            If yes, the latest message is the index 0.
	 * @param message_type       Message type to filter messages.
	 * @param custom_type        Custom type to filter messages. If filtering isn't required, set `SBD_NULL_WSTRING`.
	 * @param completion_handler The handler interface to execute.
	 */
	void GetNextMessagesByTimestamp(int64_t timestamp, int64_t next_limit, bool reverse, SBDMessageTypeFilter message_type,
		const std::wstring& custom_type, std::function<void(const std::vector<SBDBaseMessage*>&, SBDError*)> completion_handler);

	/**
	 * Gets the previous messages by the timestamp with a limit and ordering.
	 *
	 * @param timestamp          The standard timestamp to load messages.
	 * @param prev_limit         The limit for the number of messages. The returned messages could be more than this number if there are messages which have the same timestamp.
	 * @param reverse            If yes, the latest message is the index 0.
	 * @param message_type       Message type to filter messages.
	 * @param custom_type        Custom type to filter messages. If filtering isn't required, set `SBD_NULL_WSTRING`.
	 * @param completion_handler The handler interface to execute.
	 */
	void GetPreviousMessagesByTimestamp(int64_t timestamp, int64_t prev_limit, bool reverse, SBDMessageTypeFilter message_type,
		const std::wstring& custom_type, std::function<void(const std::vector<SBDBaseMessage*>&, SBDError*)> completion_handler);

	/**
	 * Gets the previous and next message by the timestamp with a limit and ordering.
	 *
	 * @param timestamp          The standard timestamp to load messages.
	 * @param prev_limit         The previous limit for the number of messages. The returned messages could be more than this number if there are messages which have the same timestamp.
	 * @param next_limit         The next limit for the number of messages. The returned messages could be more than this number if there are messages which have the same timestamp.
	 * @param reverse            If yes, the latest message is the index 0.
	 * @param message_type       Message type to filter messages.
	 * @param custom_type        Custom type to filter messages. If filtering isn't required, set `SBD_NULL_WSTRING`.
	 * @param completion_handler The handler interface to execute.
	 */
	void GetMessagesByTimestamp(int64_t timestamp, int64_t prev_limit, int64_t next_limit, bool reverse, SBDMessageTypeFilter message_type,
		const std::wstring& custom_type, std::function<void(const std::vector<SBDBaseMessage*>&, SBDError*)> completion_handler);

	/**
	 * Gets the next messages by the message ID with a limit and ordering.
	 *
	 * @param message_id         The standard message ID to load messages.
	 * @param next_limit         The limit for the number of messages. The returned messages could be more than this number if there are messages which have the same timestamp.
	 * @param reverse            If yes, the latest message is the index 0.
	 * @param message_type       Message type to filter messages.
	 * @param custom_type        Custom type to filter messages. If filtering isn't required, set `SBD_NULL_WSTRING`.
	 * @param completion_handler The handler interface to execute.
	 */
	void GetNextMessagesByMessageId(int64_t message_id, int64_t next_limit, bool reverse, SBDMessageTypeFilter message_type, const std::wstring& custom_type,
		std::function<void(const std::vector<SBDBaseMessage*>&, SBDError*)> completion_handler);

	/**
	 * Gets the previous messages by the message ID with a limit and ordering.
	 *
	 * @param message_id         The standard message ID to load messages.
	 * @param prev_limit         The limit for the number of messages. The returned messages could be more than this number if there are messages which have the same timestamp.
	 * @param reverse            If yes, the latest message is the index 0.
	 * @param message_type       Message type to filter messages.
	 * @param custom_type        Custom type to filter messages. If filtering isn't required, set `SBD_NULL_WSTRING`.
	 * @param completion_handler The handler interface to execute.
	 */
	void GetPreviousMessagesByMessageId(int64_t message_id, int64_t prev_limit, bool reverse, SBDMessageTypeFilter message_type, const std::wstring& custom_type,
		std::function<void(const std::vector<SBDBaseMessage*>&, SBDError*)> completion_handler);

	/**
	 * Gets the previous and next message by the message ID with a limit and ordering.
	 *
	 * @param message_id         The standard message ID to load messages.
	 * @param prev_limit         The previous limit for the number of messages. The returned messages could be more than this number if there are messages which have the same timestamp.
	 * @param next_limit         The next limit for the number of messages. The returned messages could be more than this number if there are messages which have the same timestamp.
	 * @param reverse            If yes, the latest message is the index 0.
	 * @param message_type       Message type to filter messages.
	 * @param custom_type        Custom type to filter messages. If filtering isn't required, set `SBD_NULL_WSTRING`.
	 * @param completion_handler The handler interface to execute.
	 */
	void GetMessagesByMessageId(int64_t message_id, int64_t prev_limit, int64_t next_limit, bool reverse, SBDMessageTypeFilter message_type,
		const std::wstring& custom_type, std::function<void(const std::vector<SBDBaseMessage*>&, SBDError*)> completion_handler);

	/**
	 * AddMessageMetaArrayValues
	 */
	void AddMessageMetaArrayValues(const std::wstring& channel_url, int64_t message_id, const std::map<std::wstring, std::vector<std::wstring>>& key_value,
		std::function<void(SBDBaseMessage*, SBDError*)> completion_handler);

	/**
	 * AddMessageMetaArrayValues
	 */
	void AddMessageMetaArrayValues(const std::wstring& channel_url, int64_t message_id, const std::vector<SBDMessageMetaArray>& metaarrays,
		std::function<void(SBDBaseMessage*, SBDError*)> completion_handler);

	/**
	 * RemoveMessageMetaArrayValues
	 */
	void RemoveMessageMetaArrayValues(const std::wstring& channel_url, int64_t message_id, const std::map<std::wstring, std::vector<std::wstring>>& key_value,
		std::function<void(SBDBaseMessage*, SBDError*)> completion_handler);

	/**
	 * RemoveMessageMetaArrayValues
	 */
	void RemoveMessageMetaArrayValues(const std::wstring& channel_url, int64_t message_id, const std::vector<SBDMessageMetaArray>& metaarrays,
		std::function<void(SBDBaseMessage*, SBDError*)> completion_handler);

	/**
	 * DeleteMessageMetaArrayKeys
	 */
	void DeleteMessageMetaArrayKeys(const std::wstring& channel_url, int64_t message_id, const std::vector<std::wstring>& metaarray_keys,
		std::function<void(SBDBaseMessage*, SBDError*)> completion_handler);

	/**
	 * CreateMessageMetaArrayKeys
	 */
	void CreateMessageMetaArrayKeys(const std::wstring& channel_url, int64_t message_id, const std::vector<std::wstring>& metaarray_keys,
		std::function<void(SBDBaseMessage*, SBDError*)> completion_handler);

	/**
	 * Copies a user message to the target channel.
	 *
	 * @param message User message object.
	 * @param target_channel Target channel object.
	 * @param completion_handler The handler interface to execute. The `userMessage` is a user message which is returned from the Sendbird server. The message has a message ID.
	 * @return Returns the temporary user message with a request ID. It doesn't have a message ID.
	 */
	SBDUserMessage* CopyUserMessage(SBDUserMessage* message, SBDBaseChannel* target_channel, std::function<void(SBDUserMessage*, SBDError*)> completion_handler);

	/**
	 * Copies a file message to the target channel.
	 *
	 * @param message File message object.
	 * @param target_channel Target channel object.
	 * @param completion_handler The handler interface to execute. The `fileMessage` is a user message which is returned from the Sendbird server. The message has a message ID.
	 * @return Returns the temporary file message with a request ID. It doesn't have a message ID.
	 */
	SBDFileMessage* CopyFileMessage(SBDFileMessage* message, SBDBaseChannel* target_channel, std::function<void(SBDFileMessage*, SBDError*)> completion_handler);

	/**
     * Add operators to the channel.
	 * 
     * @param user_ids user ids to add as operators.
     * @param completion_handler handler to register
     * @since 3.0.37
     */
    void AddOperators(const std::vector<std::wstring>& user_ids, std::function<void(SBDError*)> completion_handler);

    /**
     * Remove operators from the channel.
     *
     * @param user_ids user ids to remove from operators list.
     * @param completion_handler handler to register
     * @since 3.0.37
     */
    void RemoveOperators(const std::vector<std::wstring>& user_ids, std::function<void(SBDError*)> completion_handler);

    /**
     * Remove all operators from the channel.
     *
     * @param completion_handler handler to register
     * @since 3.0.37
     */
    void RemoveAllOperators(std::function<void(SBDError*)> completion_handler);

	/**
     * Creates a query instance to get the operator list from this channel.
     *
     * @return Query to use.
     * @since 3.0.37
     */
    SBDOperatorListQuery* CreateOperatorListQuery();

protected:
	SBDBaseChannel(const std::string& dict);
	virtual ~SBDBaseChannel() {}

private:
	static std::queue<int64_t> message_id_queue;
	static std::queue<std::wstring> message_req_id_queue;
	static std::map<int64_t, std::shared_ptr<SBDBaseMessage>> message_id_map;
	static std::map<std::wstring, std::shared_ptr<SBDBaseMessage>> message_req_id_map;
	static std::mutex message_map_mutex;
	static SBDBaseMessage* UpsertMessage(SBDBaseMessage* msg);

	static void DeallocateAllMessages();

	SBDUserMessage* SendUserMessage(const std::wstring& message, const std::wstring& data, const std::wstring& custom_type,
		const std::vector<std::wstring>& target_languages, std::function<void(SBDUserMessage*, SBDError*)> completion_handler);
	SBDFileMessage* SendFileMessage(const std::wstring& url, const std::wstring& filename, int64_t size, const std::wstring& type,
		const std::wstring& data, const std::wstring& custom_type, std::function<void(SBDFileMessage*, SBDError*)> completion_handler);
	SBDFileMessage* SendFileMessageWithPath(const std::wstring& file_path, const std::wstring& type,
		std::vector<SBDThumbnailSize> thumbnail_sizes, const std::wstring& data, const std::wstring& custom_type,
		std::function<void(SBDFileMessage*, SBDError*)> completion_handler);

	friend FSBDBaseChannel;
};

#endif /* SENDBIRD_SBDBASECHANNEL_H_ */
