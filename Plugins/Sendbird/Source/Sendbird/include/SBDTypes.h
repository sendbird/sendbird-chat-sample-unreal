// Copyright (c) 2021 Sendbird, Inc. All rights reserved.

#ifndef SENDBIRD_SBDTYPES_H_
#define SENDBIRD_SBDTYPES_H_

/**
 * Error codes.
 */
typedef enum {
	SBDErrorNone							= 0,

	// Client error codes
	SBDErrorInvalidInitialization			= 800100,
	SBDErrorConnectionRequired				= 800101,

	SBDErrorInvalidParameter				= 800110,
	SBDErrorNetworkError					= 800120,

	SBDErrorWrongChannelType				= 800150,	
	SBDErrorMarkAsReadRateLimitExceeded		= 800160,
	SBDErrorQueryInProgress					= 800170,

	SBDErrorLoginTimeout					= 800190,
	SBDErrorWebSocketConnectionClosed		= 800200,
	SBDErrorWebSocketConnectionFailed		= 800210,

	// Server error codes
	SBDErrorInternalServerError				= 500901,
} SBDErrorCode;

/**
 * Connection state.
 */
enum class SBDConnectionState {
	Connecting,
	Open,
	Closing,
	Closed,
};

/**
 * User connection statuses for `SBDUser`.
 */
enum class SBDUserConnectionStatus {
	NotAvailable,
	Online,
	Offline,
};

/**
 * Channel types.
 */
enum class SBDChannelType {
	Open,
	Group,
};

/**
 * The order type for `SBDGroupChannelListQuery`.
 */
enum class SBDGroupChannelListOrder {
	Chronological,
	LatestLastMessage,
};

/**
 * The query type for `SBDGroupChannelListQuery`.
 */
enum class SBDGroupChannelListQueryType {
	And,
	Or,
};

/**
 * Message type for filtering.
 */
enum class SBDMessageTypeFilter {
	All,
	User,
	File,
	Admin,
};

/**
 * Member state filter for group channel list query and group channel count.
 */
enum class SBDMemberStateFilter {
	All,
	JoinedOnly,
	InvitedOnly,
};

/**
 * Member state in group channel.
 */
enum class SBDMemberState {
	Joined,
	Invited,
};

/**
 * Message type.
 */
enum class SBDMessageType {
	User,
	File,
	Admin,
};

/**
 * Mention type.
 */
enum class SBDMentionType {
	Channel,
	Users,
};

/**
 * The enum type to filter my group channels with the public state.
 * 
 * @since 3.0.3
 */
enum class SBDChannelPublicStateFilter {
	All,
	Public,
	Private,
};

/**
 * The enum type to filter my group channels with the hidden state.
 */
enum class SBDChannelHiddenStateFilter {
	/**
	 * Shows the unhidden channels only.
	 */
	UnhiddenOnly,

	/**
	 * Shows the hidden channels only.
	 */
	HiddenOnly,

	/**
	 * Shows the channels will be unhidden automatically when there is a new message in the channel.
	 */
	HiddenAllowAutoUnhide,

	/**
	 * Shows the channels will not be unhidden automatically.
	 */
	HiddenPreventAutoUnhide,
};

/**
 * SBDGroupChannelHiddenState.
 */
enum class SBDGroupChannelHiddenState {
	Unhidden,
	AllowAutoUnhide,
	HiddenPreventAutoUnhide,
};

/**
 * Push option for message.
 */
enum class SBDPushNotificationDeliveryOption {
	Default,
	Suppress,
};

/**
 * Push token type.
 * 
 * @since 3.0.46
 */
enum class SBDPushTokenType {
	Fcm,
	Apns,
	ApnsVoip,
	Hms,
};

/**
 * Push token registration status.
 * 
 * @since 3.0.46
 */
enum class SBDPushTokenRegistrationStatus {
	Success,
	Pending,
	Error,
};

#endif /* SENDBIRD_SBDTYPES_H_ */
