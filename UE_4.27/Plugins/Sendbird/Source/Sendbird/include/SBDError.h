// Copyright (c) 2021 Sendbird, Inc. All rights reserved.

#ifndef SENDBIRD_SBDERROR_H_
#define SENDBIRD_SBDERROR_H_

#include <string>

class FSBDError;

/**
 * Sendbird error class.
 */
class SBDError final {
public:
	/**
	 * The error message.
	 */
	std::wstring message;

	/**
	 * The error code.
	 */
	int64_t code;

private:
	SBDError(const std::string& msg, int64_t c);

	friend FSBDError;
};

#endif /* SENDBIRD_SBDERROR_H_ */
