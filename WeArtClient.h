/**
*	WEART - Common TCP Client
*	https://www.weart.it/
*/

#pragma once
#include "WeArtMessages.h"
#include "WeArtMessageSerializer.h"
#include "WeArtThimbleTrackingObject.h"
#include "WeArtRawSensorsData.h"
#include <winsock2.h>

class WeArtClient
{
private:
	bool IsConnected = false;

	SOCKET ConnectSocket;

	std::vector<WeArtThimbleTrackingObject*> thimbleTrackingObjects;
	std::vector<WeArtRawSensorsData*> thimbleRawSensorData;

	void ForwardingMessages(std::vector<WeArtMessage*> messages);

	PCSTR IP_ADDESS; 
	PCSTR PORT;

protected:
	WeArtMessageSerializer messageSerializer;

	void OnReceive();

public:
	WeArtClient(PCSTR ip_address, PCSTR port);

	void Start();
	void Stop();

	void Run();
	void Close();

	void StartCalibration();
	void StopCalibration();

	void SendMessage(WeArtMessage* message);

	void AddThimbleTracking(WeArtThimbleTrackingObject* trackingObjects);
	void AddThimbleRawSensors(WeArtRawSensorsData* rawSensorData);

	int SizeThimbles();
};

