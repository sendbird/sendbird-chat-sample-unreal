// Copyright (c) 2021 Sendbird, Inc. All rights reserved.

#ifndef SENDBIRD_SBDMEMBER_H_
#define SENDBIRD_SBDMEMBER_H_

#include "SBDUser.h"
#include "SBDTypes.h"

#include <string>

class FSBDMember;

/**
 * The `SBDMember` class represents a member of a group channel. This class has a property to show the state for invitation.
 */
class SBDMember final : public SBDUser {
public:
	/**
	 * The state for invitation. The values of the property are `invited` and `joined`.
	 * The `invited` means that the user doesn't accept the invitation yet and the `joined` means that the user accepted the invitation manually or automatically.
	 */
	SBDMemberState state;

	/**
	 * If the member is blocked by the current logged-in user, then true.
	 */
	bool is_blocked_by_me;

	/**
	 * If the member is blocking the current logged-in user, then true.
	 */
	bool is_blocking_me;

public:
	SBDMember();

private:
	SBDMember(std::string dict);

	friend FSBDMember;
};

#endif /* SENDBIRD_SBDMEMBER_H_ */
