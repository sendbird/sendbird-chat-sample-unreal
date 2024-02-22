// Copyright (c) 2021 Sendbird, Inc. All rights reserved.

#ifndef SENDBIRD_SENDBIRD_H_
#define SENDBIRD_SENDBIRD_H_

#include "SBDAdminMessage.h"
#include "SBDBaseChannel.h"
#include "SBDBaseInterface.h"
#include "SBDBaseMessage.h"
#include "SBDError.h"
#include "SBDFileMessage.h"
#include "SBDFileMessageParams.h"
#include "SBDGroupChannel.h"
#include "SBDGroupChannelListQuery.h"
#include "SBDGroupChannelParams.h"
#include "SBDGroupChannelTotalUnreadMessageCountParams.h"
#include "SBDMember.h"
#include "SBDMessageMetaArray.h"
#include "SBDOpenChannel.h"
#include "SBDOpenChannelListQuery.h"
#include "SBDOpenChannelParams.h"
#include "SBDOperatorListQuery.h"
#include "SBDPreviousMessageListQuery.h"
#include "SBDReadStatus.h"
#include "SBDTypes.h"
#include "SBDUser.h"
#include "SBDUserListQuery.h"
#include "SBDUserMessage.h"
#include "SBDUserMessageParams.h"

#include <string>
#include <sstream>
#include <ctime>
#include <map>
#include <cstdio>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <codecvt>
#include <exception>

#define SBD_NULL_STRING "SBD_NULL_STRING_f2648f84e7e941208c8d9df02230a7c8"
#define SBD_NULL_WSTRING L"SBD_NULL_STRING_f2648f84e7e941208c8d9df02230a7c8"
#define IsNullString(str) (((str).compare(SBD_NULL_STRING)) == 0)
#define IsNullWString(str) (((str).compare(SBD_NULL_WSTRING)) == 0)

class SBDWebSocketClient;
class SBDLoginTimerHandler;
class SBDUpdateUserInfoWithBinaryFileRequestHandler;
class SBDWebSocketSendCommandHandler;
class SBDReconnectTimerHandler;
class SBDTypingStatusTimerHandler;
class SBDAckTimerHandler;
class SBDUnReadMessageCount;
class FSBDMain;
class SBDThreadManager;

/**
 * Represents operation options.
 */
class SBDOption final {
public:
	/**
	 * If set <code>true</code>, the sender information of `sender` of `SBDUserMessage` or `SBDFileMessage`
	 * such as nickname and profile url will be returned as the latest user's. Otherwise, the information will be the value of the message creation time.
	 */
	static bool use_member_as_message_sender;
};

/**
 * SBDConnectionHandler
 */
class SBDConnectionHandler {
public:
	virtual void Started() {};
	virtual void Succeeded() {};
	virtual void Failed() {};
};

/**
 * SBDUserEventHandler
 * 
 * @since 3.0.37
 */
class SBDUserEventHandler {
public:

	/**
	 * Gets the subscribed total number of unread message of all GroupChannels the current user has joined.
	 *
	 * @param total_count                The subscribed total unread message count.
	 * @param total_count_by_custom_type Total subscribed custom type unread message count.
	 * @since 3.0.37
	 */
	virtual void TotalUnreadMessageCountChanged(int total_count, std::map<std::wstring, int> total_count_by_custom_type) {}
};

/**
 * The `SBDMain` is the core class for Sendbird. This class is singletone instance which is initialized by Application ID.
 * This class provides the methods for overall. The methods include `SBDConnectionHandler` registration for receiving events are related to channels,
 * `SBDConnectionHandler` for managing the connection status, updating the current user's information, and blocking other users.
 */
class SBDMain final {
public:
	/**
	 * Gets the Application ID which was used for initialization.
	 *
	 * @return The Application ID.
	 */
	static std::wstring GetApplicationId();

	/**
	 * Retrieves the SDK version.
	 *
	 * @return The SDK version.
	 */
	static std::wstring GetSdkVersion();

	/**
	 * Gets initializing state.
	 *
	 * @return If true, `SBDMain` instance is initialized.
	 */
	static bool IsInitialized();

	/**
	 * Initializes `SBDMain` singleton instance with Sendbird Application ID. The Application ID is on Sendbird dashboard. This method has to be run first in order to user Sendbird.
	 *
	 * @param applicationId The Applicatin ID of Sendbird. It can be founded on Sendbird Dashboard.
	 */
	static void Init(std::wstring application_id);

	/**
	 * Gets the WebSocket server connection state.
	 *
	 * - `Connecting` - Connecting to the chat server
	 * - `Open` - Connected to the chat server
	 * - `Closing` - Disconnecting from the chat server
	 * - `Closed` - Disconnected from the chat server
	 * 
	 * @return `SBDConnectionState`
	 */
	static SBDConnectionState GetConnectionState();

	/**
	 * Adds the `SBDConnectionHandler`.
	 *
	 * @param identifier The identifier for the handler.
	 * @param handler    `SBDConnectionHandler` handler.
	 */
	static void AddConnectionHandler(std::wstring identifier, SBDConnectionHandler* handler);

	/**
	 * Removes the `SBDConnectionHandler` by identifier.
	 *
	 * @param identifier The identifier for the handler to be removed.
	 */
	static void RemoveConnectionHandler(std::wstring identifier);

	/**
	 * Removes all reconnection handlers.
	 */
	static void RemoveAllConnectionHandlers();

	/**
	 * Adds a user event handler. All added handlers will be notified when events occur.
	 * 
	 * @param identifier ID of handler to be added.
	 * @param handler    Handler to be added.
	 * @since 3.0.37
	 */
	static void AddUserEventHandler(std::wstring identifier, SBDUserEventHandler* handler);

	/**
	 * Removes a user event handler. The deleted handler no longer be notified.
	 * 
	 * @param identifier ID of handler to be removed.
	 * @since 3.0.37
	 */
	static void RemoveUserEventHandler(std::wstring identifier);

	/**
	 * Removes all user event handlers.
	 * 
	 * @since 3.0.37
	 */
	static void RemoveAllUserEventHandlers();

	/**
	 * Adds the `SBDChannelHandler`.
	 *
	 * @param identifier The identifier for handler.
	 * @param handler    `SBDChannelHandler` handler.
	 */
	static void AddChannelHandler(std::wstring identifier, SBDChannelHandler* handler);

	/**
	 * Removes the `SBDChannelHandler` by identifier.
	 *
	 * @param identifier The identifier for the handler to be removed.
	 */
	static void RemoveChannelHandler(std::wstring identifier);

	/**
	 * Removes all channel handlers.
	 */
	static void RemoveAllChannelHandlers();

	/**
	 * Performs a connection to Sendbird with the user ID and the access token.
	 *
	 * @param user_id            The user ID.
	 * @param access_token       The access token. If the user doesn't have access token, set nil.
	 * @param completion_handler The handler interface to execute. `user` is the object to represent the current user.
	 */
	static void Connect(std::wstring user_id, std::wstring access_token, std::function<void(SBDUser*, SBDError*)> completion_handler);

	/**
	 * Disconnects from Sendbird. If this method is invoked, the current user will be invalidated.
	 *
	 * @param completion_handler The handler interface to execute.
	 */
	static void Disconnect(std::function<void()> completion_handler);

	/**
	 * Gets the current user.
	 * 
	 * @return The current user.
	 * @since 3.0.20
	 */
	static SBDUser* GetCurrentUser();

	/**
	 * Updates the current user's information.
	 *
	 * @param nickname           New nickname.
	 * @param profile_url        New profile image url.
	 * @param completion_handler The handler interface to execute.
	 */
	static void UpdateCurrentUserInfo(std::wstring nickname, std::wstring profile_url, std::function<void(SBDError*)> completion_handler);

	/**
	 * Updates the current user's information.
	 *
	 * @param nickname                New nickname.
	 * @param profile_image_file_path New profile image file path.
	 * @param mime_type               The image file's mime type.
	 * @param completion_handler      The handler interface to execute.
	 */
	static void UpdateCurrentUserInfoWithBinaryProfileImage(std::wstring nickname, std::wstring profile_image_file_path, std::wstring mime_type,
		std::function<void(SBDError*)> completion_handler);

	/**
	 * Creates `SBDUserListQuery` instance for getting a list of all users of this application.
	 *
	 * @return `SBDUserListQuery` instance.
	 */
	static SBDUserListQuery* CreateAllUserListQuery();

	/**
	 * Creates `SBDUserListQuery` instance for gettting a list of users of this application with user IDs.
	 *
	 * @param user_ids The user IDs to get user objects.
	 * @return `SBDUserListQuery` instance.
	 */
	static SBDUserListQuery* CreateUserListQuery(std::vector<std::wstring> user_ids);

	/**
	 * Creates `SBDUserListQuery` instance for getting a list of blocked users by the current user.
	 *
	 * @return `SBDUserListQuery` instance.
	 */
	static SBDUserListQuery* CreateBlockedUserListQuery();

	/**
	 * Blocks the specified user.
	 *
	 * @param user               The user to be blocked.
	 * @param completion_handler The handler interface to execute. `blockedUser` is the blocked user by the current user.
	 */
	static void BlockUser(const SBDUser& user, std::function<void(SBDUser* blocked_user, SBDError*)> completion_handler);

	/**
	 * Unblocks the specified user.
	 *
	 * @param userId             The user ID which was blocked.
	 * @param completion_handler The handler interface to execute.
	 */
	static void UnblockUser(const SBDUser& user, std::function<void(SBDError*)> completion_handler);

	/**
	 * Starts reconnection explictly. The `SBDConnectionHandler` handlers will be invoked by the reconnection process.
	 * 
	 * @return Returns true if there is the data to be used for reconnection.
	 */
	static bool Reconnect();

	/**
	 * Sets group channel invitation preference for auto acceptance.
	 * 
	 * @param auto_accept If true, the current user will accept the group channel invitation automatically.
	 * @param completion_handler The handler interface to execute.
	 */
	static void SetChannelInvitationPreference(bool auto_accept, std::function<void(SBDError*)> completion_handler);

	/**
	 * Gets group channel inviation preference for auto acceptance.
	 * 
	 * @param completion_handler The handler interface to execute.
	 */
	static void GetChannelInvitationPreference(std::function<void(bool is_auto_accept, SBDError*)> completion_handler);

	/**
	 * Gets total unread channel count.
	 * 
	 * @param completion_handler The handler interface to execute.
	 * @since 3.0.37
	 */
	static void GetTotalUnreadChannelCount(std::function<void(int total_unread_channel_count, SBDError*)> completion_handler);

	/**
	 * Gets total unread message count.
	 * 
	 * @param params SBDGroupChannelTotalUnreadMessageCountParams.
	 * @param completion_handler The handler interface to execute.
	 * @since 3.0.37
	 */
	static void GetTotalUnreadMessageCount(SBDGroupChannelTotalUnreadMessageCountParams& params, std::function<void(int total_unread_message_count, SBDError*)> completion_handler);

    /**
     * Gets the subscribed total number of unread message of all <code>GroupChannel</code>s the current user has joined.
     *
     * @return The subscribed total unread message count.
     * @since 3.0.37
     */
    static int GetSubscribedTotalUnreadMessageCount();

    /**
     * Gets the total number of unread message of <code>GroupChannel</code>s with subscribed custom types.
     *
     * @return The subscribed custom type total unread message count.
     * @since 3.0.37
     */
    static int GetSubscribedCustomTypeTotalUnreadMessageCount();

    /**
     * Gets the number of unread message of <code>GroupChannel</code> with subscribed custom type.
     *
     * @param custom_type The subscribed custom type.
     * @return The subscribed custom type unread message count.
     * @since 3.0.37
     */
	static int GetSubscribedCustomTypeUnreadMessageCount(std::wstring custom_type);

    /**
     * Registers push token for the current <code>SBDUser</code> to receive push notification.
     * Push token registration succeeds only when the connection is made.
     * Otherwise, callback will return with <code>SBDPushTokenRegistrationStatus::Pending<code> status.
	 * Then, you can register push token again by calling this function after the connection is done.
     *
     * @param type    Push token type.
     * @param token   Push token.
     * @param unique  If true, all the previously registered token for the current <code>SBDUser</code> will be deleted and this token will be the unique one.
     * @param handler Callback handler.
     * @since 3.0.46
     */
	static void RegisterPushTokenForCurrentUser(SBDPushTokenType type, std::wstring token, bool unique, std::function<void(SBDPushTokenRegistrationStatus, SBDError*)> completion_handler);

    /**
     * Gets the pending push token.
     *
     * @return Returns the pending push token. Returns empty string if there is no registration pending token.
     * @since 3.0.46
     */
	static std::wstring GetPendingPushToken();

    /**
     * Unregisters push token for the current <code>SBDUser</code>.
     *
     * @param type    Push token type.
     * @param token   Push token.
     * @param handler Callback handler.
	 * @since 3.0.46
     */
	static void UnregisterPushTokenForCurrentUser(SBDPushTokenType type, std::wstring token, std::function<void(SBDError*)> completion_handler);

    /**
     * Unregisters all push token bound to the current <code>SBDUser</code>.
     *
     * @param handler Callback handler.
	 * @since 3.0.46
     */
    static void UnregisterPushTokenAllForCurrentUser(std::function<void(SBDError*)> completion_handler);

private:
	std::wstring application_id;
	SBDUser current_user;
	std::mutex current_user_mutex;

	std::map<std::wstring, SBDConnectionHandler*> reconnection_handlers;
	std::mutex reconnection_handlers_mutex;
	std::map<std::wstring, SBDUserEventHandler*> user_event_handlers;
	std::mutex user_event_handlers_mutex;
	std::map<std::wstring, SBDChannelHandler*> channel_handlers;
	std::mutex channel_handlers_mutex;

	std::unique_ptr<SBDWebSocketClient> websocket_client;
	std::mutex websocket_client_mutex;

	std::mutex reconnect_in_progress_mutex;
	bool reconnect_in_progress;
	SBDReconnectTimerHandler* reconnect_timer = nullptr;
	std::mutex reconnect_timer_mutex;

	SBDTypingStatusTimerHandler* typing_status_timer = nullptr;
	std::mutex typing_status_timer_mutex;

	bool session_opened;
	bool has_current_user;

	std::vector<std::unique_ptr<SBDUserListQuery>> user_list_queries;
	std::vector<std::unique_ptr<SBDUpdateUserInfoWithBinaryFileRequestHandler>> update_user_info_with_binary_file_request_handlers;

	SBDLoginTimerHandler* login_timer = nullptr;
	std::mutex login_timer_mutex;

	unsigned long reconnect_delay;
	uint8_t reconnect_count;
	std::map<std::string, SBDAckTimerHandler*> ack_timer_map;
	std::mutex ack_timer_map_mutex;

	std::vector<std::function<void(SBDUser*, SBDError*)>> connect_callbacks;
	std::mutex connect_callbacks_mutex;
	std::wstring connecting_user_id;

	std::vector<std::function<void()>> disconnect_callbacks;
	std::mutex disconnect_callbacks_mutex;
	bool is_disconnecting;

	std::unique_ptr<SBDUnReadMessageCount> un_read_message_count;
	std::unique_ptr<SBDThreadManager> thread_manager; // FEATURE_THREAD_MANAGER

	std::wstring pending_push_token;

private:
	SBDMain();
	SBDMain(SBDMain const&) = delete;
	void operator=(SBDMain const&) = delete;
	void ReceiveMessage(std::string message, bool reconnect);
	void Send(SBDCommand* command, std::function<void(SBDCommand*, SBDError*)> completion_handler);
	static SBDMain& GetInstance();
	static void Disconnect(bool logout, bool from_disconnect, std::function<void()> completion_handler);
	static void ClearUserData();
	static void ClearLocalData();

	static SBDConnectionHandler* GetReconnectionHandler(std::wstring identifier);
	static SBDChannelHandler* GetChannelHandler(std::wstring identifier);
	static std::map<std::wstring, SBDConnectionHandler*> GetReconnectionHandlers();
	static std::map<std::wstring, SBDUserEventHandler*> GetUserEventHandlers();
	static std::map<std::wstring, SBDChannelHandler*> GetChannelHandlers();

	static void ClearCurrentUser();
	static void ProcessSubscribedUnreadMessageCount(SBDCommand* command);
	static void ClearUnreadCount();
	static void Reconnect(unsigned long timeout);
	static void ConnectToWebSocketServer(bool reconnect,
		std::wstring user_id, std::wstring access_token, std::string session_key,
		std::function<void(SBDUser*, SBDError*)> connect_callback);
	static void SetHosts(std::wstring api_host, std::wstring ws_host);
	static std::wstring GetApiServer();
	static std::wstring GetCustomApiServer();
	static std::wstring GetApiHostScheme();
	static std::wstring GetApiHost();
	static int GetApiPort();
	static std::wstring GetWsServer();
	static std::wstring GetCustomWsServer();
	static std::wstring GetWsHostScheme();
	static std::wstring GetWsHost();
	static int GetWsPort();
	static SBDWebSocketClient* GetWebSocket();
	static bool HasCurrentUser();
	static std::wstring GetCurrentUserId();
	static void SetCurrentUser(const SBDUser& user);

	static void StartAckTimer(SBDCommand* command, std::function<void(SBDCommand*, SBDError*)> command_completion_handler);
	static void RunAckTimerCallback(SBDCommand* command, SBDError* error);
	static void StopAckTimer(SBDCommand* command);
	static std::map<std::string, SBDAckTimerHandler*>& GetAckTimerMap();
	static std::mutex& GetAckTimerMapMutex();
	static void InvalidateAckTimers();
	static void ProcessAckTimersToErrors();
	static void StartLoginTimer(std::wstring user_id, std::function<void(SBDUser*, SBDError*)> completion_handler);
	static void StopLoginTimer();

	static void IncreaseReconnectCount();
	static void ResetReconnectCount();
	static uint8_t GetReconnectCount();
	static unsigned long GetReconnectDelay();
	static void IncreaseReconnectDelay();
	static void ResetReconnectDelay();
	static void SetReconnectionProgress(bool is_in_progress);
	static bool GetReconnectionProgress();
	static void SetReconnectTimer(unsigned long time_interval);
	static void StopReconnectTimer();

	static void Connect(std::wstring user_id, std::wstring access_token, std::wstring api_host, std::wstring ws_host,
		std::function<void(SBDUser*, SBDError*)> completion_handler);
	static void ProcessConnectCallbacks(SBDUser* user, SBDError* error);
	static void ProcessDisconnectCallbacks(bool logout);
	static std::wstring GetPlatformName();
	static std::wstring GetPlatformVersion();
	static std::wstring GetDeviceOsName();
	static std::wstring GetDeviceOsVersion();
	static SBDThreadManager* GetThreadManager();

	friend FSBDMain;
};

#endif /* SENDBIRD_SENDBIRD_H_ */
