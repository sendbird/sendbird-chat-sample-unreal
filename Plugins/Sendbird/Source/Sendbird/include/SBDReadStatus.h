// Copyright (c) 2021 Sendbird, Inc. All rights reserved.

#ifndef SENDBIRD_SBDREADSTATUS_H_
#define SENDBIRD_SBDREADSTATUS_H_

#include "SBDUser.h"

class FSBDReadStatus;

/**
 * SBDReadStatus
 */
class SBDReadStatus final {
public:
	SBDUser reader;
	int64_t timestamp;
	std::wstring channel_url;
	std::wstring channel_type;

private:
	SBDReadStatus();
	SBDReadStatus(const std::string& dict);

	friend FSBDReadStatus;
};

#endif /* SENDBIRD_SBDREADSTATUS_H_ */
