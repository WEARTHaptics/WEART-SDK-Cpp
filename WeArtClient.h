#pragma once
#include "WeArtMessages.h""
#include "WeArtMessageSerializer.h"
#include "WeArtThimbleTrackingObject.h"
#include <winsock2.h>

class WeArtClient
{
private:
	bool IsConnected = false;

	SOCKET ConnectSocket;

	std::vector<WeArtThimbleTrackingObject*> thimbleTrackingObjects;

protected:
	WeArtMessageSerializer messageSerializer;

	void ReadData();
	void OnReceive();

public:
	WeArtClient(PCSTR IP_ADDESS, PCSTR PORT);

	void Start();
	void Stop();
	void Close();

	void SendMessage(WeArtMessage* message);

	void AddThimbleTracking(WeArtThimbleTrackingObject* trackingObjects);

};

