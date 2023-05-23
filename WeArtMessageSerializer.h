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

	//! @brief Creates a message of the right type given its type ID
	//! @param ID Type of the message to create
	//! @return the created message
	static WeArtMessage* createMessage(std::string& ID);

	//! @brief Serializes the given message to a string (ready to send on the network connection)
	//! @param message Message to serialize
	//! @return the serialized message as string
	std::string Serialize(WeArtMessage* message);

	//! @brief Serializes a strirng into a bytestream
	//! @param text The string to serialize
	//! @return the serialized string
	uint8* Serialize(std::string text);

	//! @brief Deserializes a string into the corresponding message object
	//! @param data String containing the serialized message
	//! @return the message deserialized from the given string
	WeArtMessage* Deserialize(std::string data);

	// Bytestream to std::string.

	//! @brief Converts a bytestream to string
	//! @param byteData Pointer to the data buffer to convert
	//! @param byteCount Size of the data buffer to convert
	//! @return deserializes buffer as string
	std::string Deserialize(uint8* byteData, int byteCount);
};
