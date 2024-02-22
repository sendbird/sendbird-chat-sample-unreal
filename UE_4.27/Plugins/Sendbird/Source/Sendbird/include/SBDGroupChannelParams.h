// Copyright (c) 2021 Sendbird, Inc. All rights reserved.

#ifndef SENDBIRD_SBDGROUPCHANNELPARAMS_H_
#define SENDBIRD_SBDGROUPCHANNELPARAMS_H_

#include <vector>
#include <string>

class SBDGroupChannel;

/**
 * The `SBDGroupChannelParams` class is used to create a group channel.
 * 
 * @since 3.0.3
 */
class SBDGroupChannelParams final {
public:
	/**
	 * Constructor
	 * 
	 * @since 3.0.3
	 */
	SBDGroupChannelParams();

    /**
     * Destructor
     * 
     * @since 3.0.3
     */
    ~SBDGroupChannelParams();

	/**
	 * Sets user_ids.
	 * 
	 * @param user_ids user_ids
	 * @return SBDGroupChannelParams
	 * @since 3.0.3
	 */
	SBDGroupChannelParams& SetUserIds(std::vector<std::wstring>& user_ids);

	/**
	 * Sets name.
	 * 
	 * @param name name
	 * @return SBDGroupChannelParams
	 * @since 3.0.3
	 */
	SBDGroupChannelParams& SetName(std::wstring name);

	/**
	 * Sets is_distinct.
	 * The default value is false.
	 * 
	 * @param is_distinct is_distinct
	 * @return SBDGroupChannelParams
	 * @since 3.0.3
	 */
	SBDGroupChannelParams& SetDistinct(bool is_distinct);

	/**
	 * Sets is_public.
	 * The default value is false.
	 * 
	 * @param is_public is_public
	 * @since 3.0.3
	 */
	SBDGroupChannelParams& SetPublic(bool is_public);

	/**
	 * Sets channel_url.
	 * 
	 * @param channel_url channel_url
	 * @return SBDGroupChannelParams
	 * @since 3.0.3
	 */
	SBDGroupChannelParams& SetChannelUrl(std::wstring channel_url);

	/**
	 * Sets cover_url.
	 * If set, the cover_image_file_path and file_mime_type will be ignored.
	 * 
	 * @param cover_url cover_url
	 * @return SBDGroupChannelParams
	 * @since 3.0.3
	 */
	SBDGroupChannelParams& SetCoverUrl(std::wstring cover_url);

	/**
	 * Sets cover_image_file_path and file_mime_type.
	 * If set, cover_url will be ignored.
	 * 
	 * @param cover_image_file_path cover_image_file_path
	 * @param file_mime_type file_mime_type
	 * @return SBDGroupChannelParams
	 * @since 3.0.3
	 */
	SBDGroupChannelParams& SetCoverImageFilePathAndFileMimeType(std::wstring cover_image_file_path, std::wstring file_mime_type);

	/**
	 * Sets data.
	 * 
	 * @param data data
	 * @return SBDGroupChannelParams
	 * @since 3.0.3
	 */
	SBDGroupChannelParams& SetData(std::wstring data);

	/**
	 * Sets custom_type.
	 * 
	 * @param custom_type custom_type
	 * @return SBDGroupChannelParams
	 * @since 3.0.3
	 */
	SBDGroupChannelParams& SetCustomType(std::wstring custom_type);

	/**
	 * Sets operator_user_ids.
	 * 
	 * @param operator_user_ids operator_user_ids
	 * @return SBDGroupChannelParams
	 * @since 3.0.37
	 */
	SBDGroupChannelParams& SetOperatorUserIds(std::vector<std::wstring>& operator_user_ids);

private:
	std::vector<std::wstring> user_ids;
	std::wstring name;
	bool is_distinct;
	bool is_public;
	std::wstring channel_url;

	// (cover_url) or (cover_image_file_path & file_mime_type)
	std::wstring cover_url;
	std::wstring cover_image_file_path;
	std::wstring file_mime_type;

	std::wstring data;
	std::wstring custom_type;
	std::vector<std::wstring> operator_user_ids;

	friend SBDGroupChannel;
};

#endif /* SENDBIRD_SBDGROUPCHANNELPARAMS_H_ */
