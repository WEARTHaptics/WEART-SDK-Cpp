#include "MiddlewareStatusListener.h"

void MiddlewareStatusListener::AddStatusCallback(std::function<void(MiddlewareStatusUpdate)> callback)
{
	_statusCallbacks.push_back(callback);
}

MiddlewareStatusUpdate MiddlewareStatusListener::lastStatus()
{
	return _data;
}

void MiddlewareStatusListener::OnMessageReceived(WeArtMessage* message)
{
	
	try {
		bool toUpdate = false;
		if (message->getID() == MiddlewareStatusMessage::ID) {
			MiddlewareStatusMessage* mwStatus = dynamic_cast<MiddlewareStatusMessage*>(message);
			if (mwStatus != nullptr) {
				MiddlewareStatusData newStatus = mwStatus->data();
				_data.timestamp = mwStatus->timestamp();
				_data.status = newStatus.status;
				_data.version = newStatus.version;
				_data.statusCode = newStatus.statusCode;
				_data.errorDesc = newStatus.errorDesc;
				_data.actuationsEnabled = newStatus.actuationsEnabled;
				toUpdate = true;
			}
		}
		else if (message->getID() == DevicesStatusMessage::ID) {
			DevicesStatusMessage* devicesStatus = dynamic_cast<DevicesStatusMessage*>(message);
			if (devicesStatus != nullptr) {
				_data.timestamp = devicesStatus->timestamp();
				_data.devices = devicesStatus->devices();
				toUpdate = true;
			}
		}

		if (toUpdate) {
			for (auto callback : _statusCallbacks) {
				callback(_data);
			}
		}
	}
	catch (const std::bad_cast&) {}
}