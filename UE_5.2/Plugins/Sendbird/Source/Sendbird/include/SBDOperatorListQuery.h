// Copyright (c) 2021 Sendbird, Inc. All rights reserved.

#ifndef SENDBIRD_SBDOPERATORLISTQUERY_H_
#define SENDBIRD_SBDOPERATORLISTQUERY_H_

#include "SBDUser.h"

#include <functional>
#include <vector>

class SBDBaseChannel;
class SBDError;

/**
 * A class representing query to retrieve operator lists related to `User`.
 *
 * @since 3.0.37
 */
class SBDOperatorListQuery final {
public:
    /**
     * Sets the maximum number of operators per queried page.
     *
     * @param limit The maximum number of operators per page.
     * @since 3.0.37
     */
    void SetLimit(int limit);

    /**
     * Checks if there is a next page.
     *
     * @return <code>true</code> if there is a next page.
     * @since 3.0.37
     */
    bool HasNext();

    /**
     * Checks if the current query is in communication progress with server.
     *
     * @return <code>true</code> if the current query is not finished.
     * @since 3.0.37
     */
    bool IsLoading();

    /**
     * Gets the list of operators.
     *
     * @param completion_handler A callback handler to get the queried result.
     * @since 3.0.37
     */
	void LoadNextPage(std::function<void(const std::vector<SBDUser>& users, SBDError* error)> completion_handler);

private:
    SBDBaseChannel* channel;
	int64_t limit;
	bool has_next;
	bool is_loading;
    std::string token;

	SBDOperatorListQuery(SBDBaseChannel* channel);

    friend SBDBaseChannel;
};

#endif /* SENDBIRD_SBDOPERATORLISTQUERY_H_ */
