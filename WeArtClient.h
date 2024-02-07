/**
*	WEART - Common TCP Client
*	https://www.weart.it/
*/

#pragma once
#include "WeArtCommon.h"
#include "WeArtMessageListener.h"
#include "WeArtMessages.h"
#include "WeArtMessageSerializer.h"
#include "WeArtThimbleTrackingObject.h"
#include "WeArtTrackingRawData.h"
#include "WeArtAnalogSensorData.h"
#include <winsock2.h>
#include <future>
#include <forward_list>

//! @brief Weart client, used to connect to the Weart middleware, perform operations and receive messages
class WeArtClient {
public:
	using MessageCallback = std::function<void(WeArtMessage*)>;

	WeArtClient(PCSTR ip_address, PCSTR port);

	//! @brief Send a start command to the middleware
	//! @param trackType Type of tracking requested
	void Start(TrackingType trackType = TrackingType::WEART_HAND);

	//! @brief Send a stop command to the middleware
	void Stop();

	//! @brief Starts and runs the network connection thread
	void Run();

	//! @brief Tells whether the client is connected to the Weart middleware
	//! @return true if the client is connected, false otherwise
	bool IsConnected();

	//! @brief Close the network connection
	void Close();

	//! @brief Starts the device calibration procedure
	void StartCalibration();

	//! @brief Stops the device calibration procedure
	void StopCalibration();

	//! @brief Ask the middleware to send raw sensors data to the sdk
	void StartRawData();

	//! @brief Ask the middleware to stop sending raw sensors data to the sdk
	void StopRawData();

	//! @brief Sends a message to the Weart middleware
	//! @param message Message to send
	void SendMessage(WeArtMessage* message);

	//! @brief Adds a thimble tracking object to the client, allowing it to receive messages
	//! @param trackingObjects Thimble tracking object to add
	void AddThimbleTracking(WeArtThimbleTrackingObject* trackingObjects);

	//! @brief Adds a raw data sensors observer to the client, allowing it to receive messages
	//! @param trackingObjects Raw sensors data object to add
	void AddThimbleRawSensors(WeArtTrackingRawData* rawSensorData);

	//! @brief Adds a analog raw data sensors observer to the client, allowing it to receive messages
	//! @param analogSensorData Raw sensors data object to add
	void AddThimbleAnalogRawSensor(WeArtAnalogSensorData *analogRawSensorData);

	//! @return The number of thimbles added to the connection
	int SizeThimbles();

	//! @brief Adds a message listener object to the client, allowing it to receive messages
	//! @param listener The listener to add
	void AddMessageListener(WeArtMessageListener* listener);

	//! @brief Adds a message callback called when a message is received 
	//! @param callback Callback to add
	void AddMessageCallback(MessageCallback callback);

	//! @brief Removes the given listener from the client. From now on the listener will not receive messages
	//! @param listener The listener to remove from the client
	void RemoveMessageListener(WeArtMessageListener* listener);

	//! @brief Remove a given callback from the client
	//! @param callback Callback to remove
	void RemoveMessageCallback(MessageCallback callback);

	//! @brief Adds a callback for the connection status (true = connected, false = disconnected)
	//! @param callback The callback to be called whenever the connection status changes
	void AddConnectionStatusCallback(std::function<void(bool)> callback);

	enum ErrorType {
		ConnectionError,
		SendMessageError,
		ReceiveMessageError
	};

	//! @brief Adds a callback for errors (connection, send or receive errors)
	//! @param callback The callback to be called whenever an error occurs
	void AddErrorCallback(std::function<void(WeArtClient::ErrorType)> callback);

protected:
	WeArtMessageSerializer messageSerializer;

	void OnReceive();

private:
	bool Connected = false;

	SOCKET ConnectSocket;

	std::vector<WeArtThimbleTrackingObject*> thimbleTrackingObjects;
	std::vector<WeArtMessageListener*> messageListeners;
	std::vector<MessageCallback> messageCallbacks;

	void ForwardingMessages(std::vector<WeArtMessage*> messages);

	// Connection status and errors callbacks management
	std::vector<std::function<void(bool)>> connectionStatusCallbacks;
	std::vector<std::function<void(ErrorType)>> errorCallbacks;
	std::forward_list<std::future<void>> pendingCallbacks;

	void NotifyConnectionStatus(bool connected);
	void NotifyError(ErrorType errorType);
	void ClearProcessedCallbacks();
	
	PCSTR IP_ADDESS;
	PCSTR PORT;
};

