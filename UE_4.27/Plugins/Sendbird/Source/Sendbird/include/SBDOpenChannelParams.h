// Copyright (c) 2021 Sendbird, Inc. All rights reserved.

#ifndef SENDBIRD_SBDOPENCHANNELPARAMS_H_
#define SENDBIRD_SBDOPENCHANNELPARAMS_H_

#include <vector>
#include <string>

class SBDOpenChannel;

/**
 * The `SBDOpenChannelParams` class is used to create a open channel.
 * 
 * @since 3.0.37
 */
class SBDOpenChannelParams final {
public:
	/**
	 * Constructor
	 * 
	 * @since 3.0.37
	 */
	SBDOpenChannelParams();

	/**
	 * Destructor
	 * 
	 * @since 3.0.37
	 */
	~SBDOpenChannelParams();

	/**
	 * Sets name.
	 * 
	 * @param name name
	 * @return SBDOpenChannelParams
	 * @since 3.0.37
	 */
	SBDOpenChannelParams& SetName(std::wstring name);

	/**
	 * Sets channel_url.
	 * 
	 * @param channel_url channel_url
	 * @return SBDOpenChannelParams
	 * @since 3.0.37
	 */
	SBDOpenChannelParams& SetChannelUrl(std::wstring channel_url);

	/**
	 * Sets cover_url.
	 * If set, the cover_image_file_path and file_mime_type will be ignored.
	 * 
	 * @param cover_url cover_url
	 * @return SBDOpenChannelParams
	 * @since 3.0.37
	 */
	SBDOpenChannelParams& SetCoverUrl(std::wstring cover_url);

	/**
	 * Sets cover_image_file_path and file_mime_type.
	 * If set, cover_url will be ignored.
	 * 
	 * @param cover_image_file_path cover_image_file_path
	 * @param file_mime_type file_mime_type
	 * @return SBDOpenChannelParams
	 * @since 3.0.37
	 */
	SBDOpenChannelParams& SetCoverImageFilePathAndFileMimeType(std::wstring cover_image_file_path, std::wstring file_mime_type);

	/**
	 * Sets data.
	 * 
	 * @param data data
	 * @return SBDOpenChannelParams
	 * @since 3.0.37
	 */
	SBDOpenChannelParams& SetData(std::wstring data);

	/**
	 * Sets custom_type.
	 * 
	 * @param custom_type custom_type
	 * @return SBDOpenChannelParams
	 * @since 3.0.37
	 */
	SBDOpenChannelParams& SetCustomType(std::wstring custom_type);

    /**
     * Sets operator_user_ids.
     * 
	 * @param operator_user_ids operator_user_ids
     * @return SBDOpenChannelParams
     * @since 3.0.37
     */
    SBDOpenChannelParams& SetOperatorUserIds(std::vector<std::wstring>& operator_user_ids);

private:
	std::wstring name;
	std::wstring channel_url;

	// (cover_url) or (cover_image_file_path & file_mime_type)
	std::wstring cover_url;
	std::wstring cover_image_file_path;
	std::wstring file_mime_type;

	std::wstring data;
	std::wstring custom_type;
	std::vector<std::wstring> operator_user_ids;

	friend SBDOpenChannel;
};

#endif /* SENDBIRD_SBDOPENCHANNELPARAMS_H_ */
