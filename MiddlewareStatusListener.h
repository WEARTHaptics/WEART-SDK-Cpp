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

	//! @brief Latest warning code sent by the middleware (0 = OK)
	int warningCode;

	//! @brief Description of the latest warning code received
	std::string warningDesc;

	//! @brief Tells if the middleware will propagate actuations to the connected devices
	bool actuationsEnabled;

	//! @brief Tells the connection type between the WeArtApp and the device.
	//! @note This is a TouchDIVER Pro only field.
	std::string connectionType;

	//! @brief Tells if the WeArtApp will automatically connect to the device.	
	//! @note This is a TouchDIVER Pro only field.
	bool autoconnection;

	//! @brief Whether the tracking playback is enabled.
	//! @note This is a TouchDIVER Pro only field.
	bool trackingPlayback;

	//! @brief Tells if the WeArtApp will log raw data.
	//! @note This is a TouchDIVER Pro only field.
	bool rawDataLog;

	//! @brief Whether the sensor on mask is enabled.	
	//! @note This is a TouchDIVER Pro only field.
	bool sensorOnMask;

	//! @brief Status and informations of the devices (TouchDIVERs and TouchDIVERs Pro) connected to the middleware.
	//! @note This field is available for both TouchDIVER and TouchDIVER Pro devices.
	std::vector<MiddlewareConnectedDevice> connectedDevices;

	//! @brief Status and informations of the devices (TouchDIVERs) connected to the middleware.
	//! @note This field is only available for TouchDIVER devices.
	std::vector <ConnectedDeviceStatus> devices;

	//! @brief Status and informations of the devices (TouchDIVERs PRO) connected to the middleware.
	//! @note This field is only available for TouchDIVER Pro devices.
	std::vector <ConnectedG2DeviceStatus> G2Devices;
};

//! @brief Listens and notifies about middleware status changes
class MiddlewareStatusListener : public WeArtMessageListener {
public:
	MiddlewareStatusListener() : WeArtMessageListener({ MiddlewareStatusMessage::ID, WeArtAppStatusMessage::ID, DevicesStatusMessage::ID, TDProStatusMessage::ID }) {}

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