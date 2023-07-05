#include "MiddlewareStatusListener.h"

void MiddlewareStatusListener::AddStatusCallback(std::function<void(MiddlewareStatusData)> callback)
{
	_statusCallbacks.push_back(callback);
}

void MiddlewareStatusListener::AddDevicesCallback(std::function<void(std::vector<ConnectedDeviceStatus>)> callback)
{
	_devicesCallbacks.push_back(callback);
}

MiddlewareStatusData MiddlewareStatusListener::lastStatus()
{
	return _data;
}

std::vector<ConnectedDeviceStatus> MiddlewareStatusListener::devices()
{
	return _devices;
}

void MiddlewareStatusListener::OnMessageReceived(WeArtMessage* message)
{
	
	try {
		if (message->getID() == MiddlewareStatusMessage::ID) {
			MiddlewareStatusMessage* mwStatus = dynamic_cast<MiddlewareStatusMessage*>(message);
			if (mwStatus != nullptr) {
				_data = mwStatus->data();
				for (auto callback : _statusCallbacks) {
					callback(_data);
				}
			}
		}
		else if (message->getID() == DevicesStatusMessage::ID) {
			DevicesStatusMessage* devicesStatus = dynamic_cast<DevicesStatusMessage*>(message);
			if (devicesStatus != nullptr) {
				_devices = devicesStatus->devices();
				for (auto callback : _devicesCallbacks) {
					callback(_devices);
				}
			}
		}
	}
	catch (const std::bad_cast&) {}
}