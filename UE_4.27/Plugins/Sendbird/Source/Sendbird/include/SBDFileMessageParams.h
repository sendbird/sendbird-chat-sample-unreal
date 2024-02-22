// Copyright (c) 2021 Sendbird, Inc. All rights reserved.

#ifndef SENDBIRD_SBDFILEMESSAGEPARAMS_H_
#define SENDBIRD_SBDFILEMESSAGEPARAMS_H_

#include "SBDBaseChannel.h"
#include "SBDFileMessage.h"

#include <vector>
#include <string>

/**
 * The `SBDFileMessageParams` class is used to send a file message in `SBDBaseChannel`.
 * 
 * @since 3.0.37
 */
class SBDFileMessageParams final {
public:
	/**
	 * Constructor
	 * 
	 * @since 3.0.37
	 */
	SBDFileMessageParams();

	/**
	 * Destructor
	 * 
	 * @since 3.0.37
	 */
	~SBDFileMessageParams();

	/**
	 * Sets file_url.
	 * If set, the file_path will be ignored.
	 * 
	 * @param file_url file_url
	 * @return SBDFileMessageParams
	 * @since 3.0.37
	 */
	SBDFileMessageParams& SetFileUrl(std::wstring file_url);

    /**
	 * Sets file_path.
	 * If set, the file_url will be ignored.
	 * 
	 * @param file_path file_path
	 * @return SBDFileMessageParams
	 * @since 3.0.37
	 */
	SBDFileMessageParams& SetFilePath(std::wstring file_path);

	/**
	 * Sets file_name.
	 * 
	 * @param file_name file_name
	 * @return SBDFileMessageParams
	 * @since 3.0.37
	 */
	SBDFileMessageParams& SetFileName(std::wstring file_name);

	/**
	 * Sets mime_type.
	 * 
	 * @param mime_type mime_type
	 * @return SBDFileMessageParams
	 * @since 3.0.37
	 */
	SBDFileMessageParams& SetMimeType(std::wstring mime_type);

	/**
	 * Sets file_size.
	 * 
	 * @param file_size file_size
	 * @return SBDFileMessageParams
	 * @since 3.0.37
	 */
	SBDFileMessageParams& SetFileSize(int64_t file_size);

	/**
	 * Sets thumbnail_sizes.
	 * 
	 * @param thumbnail_sizes thumbnail_sizes
	 * @return SBDFileMessageParams
	 * @since 3.0.37
	 */
	SBDFileMessageParams& SetThumbnailSizes(std::vector<SBDThumbnailSize>& thumbnail_sizes);

	/**
	 * Sets data.
	 * 
	 * @param data data
	 * @return SBDFileMessageParams
	 * @since 3.0.37
	 */
	SBDFileMessageParams& SetData(std::wstring data);

	/**
	 * Sets custom_type.
	 * 
	 * @param custom_type custom_type
	 * @return SBDFileMessageParams
	 * @since 3.0.37
	 */
	SBDFileMessageParams& SetCustomType(std::wstring custom_type);

private:
	// (file_url) or (file_path)
	std::wstring file_url;
	std::wstring file_path;

	std::wstring file_name;
	std::wstring mime_type;
	int64_t file_size;

	std::vector<SBDThumbnailSize> thumbnail_sizes;

	std::wstring data;
	std::wstring custom_type;

	friend SBDBaseChannel;
};

#endif /* SENDBIRD_SBDFILEMESSAGEPARAMS_H_ */
