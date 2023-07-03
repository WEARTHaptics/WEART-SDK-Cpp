#pragma once
#include "WeArtCommon.h"
#include "WeArtMessageListener.h"
#include "WeArtMessages.h"
#include <mutex>

//! @brief Listens and notifies about middleware status changes
class MiddlewareStatusListener : public WeArtMessageListener {
public:
	MiddlewareStatusListener() : WeArtMessageListener({ MiddlewareStatusMessage::ID, DevicesStatusMessage::ID }) {}

	virtual void OnMessageReceived(WeArtMessage* message) override;

	void AddStatusCallback(std::function<void(MiddlewareStatusData)> callback);

	MiddlewareStatusData lastStatus();

	std::vector<ConnectedDeviceStatus> devices();

private:
	std::vector<std::function<void(MiddlewareStatusData)>> _callbacks;
	MiddlewareStatusData _data;
	std::vector<ConnectedDeviceStatus> _devices;
};