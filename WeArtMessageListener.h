#pragma once
#include <vector>
#include <set>
#include <string>
#include <winsock2.h>
#include "WeArtMessages.h"

//! @brief Message Listener basic interface
class WeArtMessageListener {
public:

	//! @brief Basic message listener constructor
	//! @param ids List of the ID of accepted messages
	WeArtMessageListener(std::set<std::string> ids) : acceptedIds(ids) {}

	//! @brief Tells if a given ID can be processed by the listener
	//! @param id ID of the message to process
	//! @return true if the message is accepted by the listener, false otherwise
	bool accept(std::string id) {
		return acceptedIds.find(id) != acceptedIds.end();
	}

	//! @brief Callback called when an accepted message is received
	//! @param message Received message
	virtual void OnMessageReceived(WeArtMessage* message) = 0;

protected:
	std::set<std::string> acceptedIds;
};