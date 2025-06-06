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
				_data.connectedDevices = newStatus.connectedDevices;
				toUpdate = true;
			}
		}
		
		else if (message->getID() == WeArtAppStatusMessage::ID) {
			WeArtAppStatusMessage* waStatus = dynamic_cast<WeArtAppStatusMessage*>(message);
			if (waStatus != nullptr) {
				MiddlewareStatusData newStatus = waStatus->data();
				_data.timestamp = waStatus->timestamp();
				_data.status = newStatus.status;
				_data.version = newStatus.version;
				_data.statusCode = newStatus.statusCode;
				_data.errorDesc = newStatus.errorDesc;
				_data.warningCode = newStatus.warningCode;
				_data.warningDesc = newStatus.warningDesc;
				_data.actuationsEnabled = newStatus.actuationsEnabled;
				_data.connectionType = newStatus.connectionType;
				_data.autoconnection = newStatus.autoconnection;
				_data.trackingPlayback = newStatus.trackingPlayback;
				_data.rawDataLog = newStatus.rawDataLog;
				_data.sensorOnMask = newStatus.sensorOnMask;
				_data.connectedDevices = newStatus.connectedDevices;
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

		else if (message->getID() == TDProStatusMessage::ID) {
			TDProStatusMessage* tdProStatus = dynamic_cast<TDProStatusMessage*>(message);
			if (tdProStatus != nullptr) {
				_data.timestamp = tdProStatus->timestamp();
				_data.G2Devices = tdProStatus->devices();
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