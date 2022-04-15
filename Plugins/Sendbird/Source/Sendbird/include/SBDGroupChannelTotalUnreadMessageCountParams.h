// Copyright (c) 2021 Sendbird, Inc. All rights reserved.

#ifndef SENDBIRD_SBDGROUPCHANNELTOTALUNREADMESSAGECOUNTPARAMS_H_
#define SENDBIRD_SBDGROUPCHANNELTOTALUNREADMESSAGECOUNTPARAMS_H_

#include <vector>
#include <string>

class SBDMain;

/**
 * Represents a group channel total unread message count params.
 * 
 * @since 3.0.37
 */
class SBDGroupChannelTotalUnreadMessageCountParams final {
public:
	/**
	 * Constructor
	 * 
	 * @since 3.0.37
	 */
	SBDGroupChannelTotalUnreadMessageCountParams();

	/**
	 * Destructor
	 * 
	 * @since 3.0.37
	 */
	~SBDGroupChannelTotalUnreadMessageCountParams();

	/**
	 * Set channel custom types.
	 * 
	 * @since 3.0.37
	 */
	SBDGroupChannelTotalUnreadMessageCountParams& SetChannelCustomTypesFilter(std::vector<std::wstring>& channel_custom_types);

private:
	std::vector<std::wstring> channel_custom_types;

	friend SBDMain;
};

#endif /* SENDBIRD_SBDGROUPCHANNELTOTALUNREADMESSAGECOUNTPARAMS_H_ */
