#pragma once
#include <WEART_SDK/WeArtMessages.h>
#include <WEART_SDK/WeArtMessageSerializer.h>
#include <winsock2.h>

class WeArtClient
{
private:
	bool IsConnected = false;

	SOCKET ConnectSocket;

protected:
	WeArtMessageSerializer messageSerializer;


public:
	WeArtClient(PCSTR IP_ADDESS, PCSTR PORT);

	void Start();
	void Stop();
	void Close();

	void SendMessage(WeArtMessage* message);
	bool ReceiveMessages(std::vector<WeArtMessage*>& messages);
};

