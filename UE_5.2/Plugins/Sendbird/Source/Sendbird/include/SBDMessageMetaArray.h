// Copyright (c) 2021 Sendbird, Inc. All rights reserved.

#ifndef SENDBIRD_SBDMESSAGEMETAARRAY_H_
#define SENDBIRD_SBDMESSAGEMETAARRAY_H_

#include <string>
#include <vector>

class FSBDMessageMetaArray;

/**
 * SBDMessageMetaArray
 */
class SBDMessageMetaArray final {
private:

public:
	/**
	 * GetKey 
	 * 
	 * @return key
	 */
	std::wstring GetKey() const;

	/**
	 * GetValue
	 * 
	 * @return value
	 */
	std::vector<std::wstring> GetValue() const;

	/**
	 * SetKey 
	 */
	void SetKey(std::wstring key);

	/**
	 * SetValue 
	 */
	void SetValue(std::vector<std::wstring> value);

	/**
	 * AddValue 
	 */
	void AddValue(std::wstring value);

	/**
	 * AddValue 
	 */
	void AddValue(std::vector<std::wstring> value);

	/**
	 * RemoveValue 
	 */
	void RemoveValue(std::wstring value);

	/**
	 * RemoveValue 
	 */
	void RemoveValue(std::vector<std::wstring> value);

private:
	std::wstring key;
	std::vector<std::wstring> value;

	SBDMessageMetaArray(std::wstring key);
	SBDMessageMetaArray(std::wstring key, const std::vector<std::wstring>& value);

	friend FSBDMessageMetaArray;
};
#endif /* SENDBIRD_SBDMESSAGEMETAARRAY_H_ */
