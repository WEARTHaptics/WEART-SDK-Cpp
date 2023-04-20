/**
*	WEART - Common TCP Client
*	https://www.weart.it/
*/

#pragma once
#include "WeArtMessageListener.h"
#include "WeArtMessages.h"
#include "WeArtMessageSerializer.h"
#include "WeArtThimbleTrackingObject.h"
#include "WeArtRawSensorsData.h"
#include <winsock2.h>

//! @brief Weart client, used to connect to the Weart middleware, perform operations and receive messages
class WeArtClient {
public:
	WeArtClient(PCSTR ip_address, PCSTR port);

	//! @brief Send a start command to the middleware
	void Start();

	//! @brief Send a stop command to the middleware
	void Stop();

	//! @brief Starts and runs the network connection thread
	void Run();

	//! @brief Close the network connection
	void Close();

	//! @brief Starts the device calibration procedure
	void StartCalibration();

	//! @brief Stops the device calibration procedure
	void StopCalibration();

	//! @brief Sends a message to the Weart middleware
	//! @param message Message to send
	void SendMessage(WeArtMessage* message);

	//! @brief Adds a thimble tracking object to the client, allowing it to receive messages
	//! @param trackingObjects Thimble tracking object to add
	void AddThimbleTracking(WeArtThimbleTrackingObject* trackingObjects);

	//! @brief Adds a raw data sensors observer to the client, allowing it to receive messages
	//! @param trackingObjects Raw sensors data object to add
	void AddThimbleRawSensors(WeArtRawSensorsData* rawSensorData);

	//! @return The number of thimbles added to the connection
	int SizeThimbles();

	//! @brief Adds a message listener object to the client, allowing it to receive messages
	//! @param listener The listener to add
	void AddMessageListener(WeArtMessageListener* listener);

	//! @brief Removes the given listener from the client. From now on the listener will not receive messages
	//! @param listener The listener to remove from the client
	void RemoveMessageListener(WeArtMessageListener* listener);

protected:
	WeArtMessageSerializer messageSerializer;

	void OnReceive();

private:
	bool IsConnected = false;

	SOCKET ConnectSocket;

	std::vector<WeArtThimbleTrackingObject*> thimbleTrackingObjects;
	std::vector<WeArtMessageListener*> messageListeners;

	void ForwardingMessages(std::vector<WeArtMessage*> messages);

	PCSTR IP_ADDESS;
	PCSTR PORT;
};

