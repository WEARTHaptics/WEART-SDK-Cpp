#pragma once

#include "WeArtMessageSerializer.h"
#include <string>

//! @private
enum WeArtMessageType
{
	MessageSent, MessageReceived
};

//! @private
enum WeArtError
{
	ConnectionError, SendMessageError, ReceiveMessageError
};

//! @private
static char messagesSeparator = '~';

//! @private
class  UWeArtController 
{

public:

	void PauseController();
	void UnpauseController();

	void ScheduleController();

	void SendMessage(WeArtMessage* message);
	bool ReceiveMessages(std::vector<WeArtMessage*>& messages);

	void OnMessageReceived(WeArtMessage* msg);
	void StopConnection();

	void LogMessage(WeArtMessageType type, std::string& message);
	void LogError(WeArtError error, std::string& message);

protected:

		int clientPort;
		bool debugMessages;
		bool controllerAutoStart;

	bool isCancellationRequested = false;
	WeArtMessageSerializer messageSerializer;
	uint8* messageReceivedBuffer[1024];
	std::string trailingText;
	bool IsConnected = false;
	bool IsPaused = false;
};
