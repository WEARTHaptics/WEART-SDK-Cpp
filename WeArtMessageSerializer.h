#pragma once

#include "WeArtMessages.h"
#include <sstream>

//! @private
class WeArtMessageSerializer {
public:
	const char separator = ':';

	static constexpr unsigned long long int HashStringToInt(const char* str, unsigned long long int hash = 0) {
		return (*str == 0) ? hash : 101 * HashStringToInt(str + 1) + *str;
	}

	//! @brief Deserializes a string into the corresponding message object
	//! @param data String containing the serialized message
	//! @return the message deserialized from the given string
	WeArtMessage* Deserialize(std::string data);

private:

	std::string extractID(std::string& data);

	//! @brief Creates a message of the right type given its type ID
	//! @param ID	String containing the message id
	//! @return the created message
	WeArtMessage* createMessage(std::string& ID);
};
