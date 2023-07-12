#pragma once
#include "WeArtCommon.h"
#include "WeArtMessageListener.h"
#include "WeArtMessages.h"
#include <mutex>

//! @brief Middleware status fields
struct MiddlewareStatusUpdate {
	//! @brief Timestamp of the last status update received by the middleware
	std::uint64_t timestamp;
	
	//! @brief Current status of the middleware
	MiddlewareStatus status;

	//! @brief Middleware version
	std::string version;

	//! @brief Latest status code sent by the middleware (0 = OK)
	int statusCode;

	//! @brief Description of the latest status code received
	std::string errorDesc;

	//! @brief Tells if the middleware will propagate actuations to the connected devices
	bool actuationsEnabled;

	//! @brief Status and informations of the devices (TouchDIVERs) connected to the middleware
	std::vector<ConnectedDeviceStatus> devices;
};

//! @brief Listens and notifies about middleware status changes
class MiddlewareStatusListener : public WeArtMessageListener {
public:
	MiddlewareStatusListener() : WeArtMessageListener({ MiddlewareStatusMessage::ID, DevicesStatusMessage::ID }) {}

	//! @copydoc WeArtMessageListener::OnMessageReceived
	virtual void OnMessageReceived(WeArtMessage* message) override;

	//! @brief Adds a callback notified on every new status update from the middleware
	//! @param callback Method called when the sdk receives a status update from the middleware
	void AddStatusCallback(std::function<void(MiddlewareStatusUpdate)> callback);

	//! @brief Gets the last status received
	//! @return last status received
	MiddlewareStatusUpdate lastStatus();

private:
	std::vector<std::function<void(MiddlewareStatusUpdate)>> _statusCallbacks;
	MiddlewareStatusUpdate _data;
};