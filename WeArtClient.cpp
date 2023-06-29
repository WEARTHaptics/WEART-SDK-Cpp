/**
*	WEART - Common TCP Client
*	https://www.weart.it/
*/

#include "WeArtClient.h"
#include <iostream>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <ws2tcpip.h>
#include <windows.h>


#include <ppltasks.h>
using namespace Windows::Foundation;

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_BUFLEN 512
int recvbuflen = DEFAULT_BUFLEN;
char recvbuf[DEFAULT_BUFLEN];

#define DATA_BUFSIZE 4096

static char messagesSeparator = '~';

// Helper methods to format error codes
std::string WSAGetLastErrorString();
std::string FormatWSAStartupError(int result);

// https://docs.microsoft.com/en-us/windows/win32/winsock/complete-client-code
// reference
WeArtClient::WeArtClient(PCSTR ip_address, PCSTR port) {
	IP_ADDESS = ip_address;
	PORT = port;
}

void WeArtClient::Run() {

	WSADATA wsaData;
	ConnectSocket = INVALID_SOCKET;
	struct addrinfo* result = NULL,
		* ptr = NULL,
		hints;

	int iResult;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		Connected = false;
		std::cerr << "WSAStartup failed with error: \"" << FormatWSAStartupError(iResult) << "\"" << std::endl;
		NotifyError(ErrorType::ConnectionError);
		return;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	iResult = getaddrinfo(IP_ADDESS, PORT, &hints, &result);
	if (iResult != 0) {
		Connected = false;
		std::cerr << "getaddrinfo failed with error: " << iResult << std::endl;
		WSACleanup();
		NotifyError(ErrorType::ConnectionError);
		return;
	}

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			Connected = false;
			std::cerr << "socket failed with error: " << WSAGetLastErrorString() << std::endl;
			WSACleanup();
			NotifyError(ErrorType::ConnectionError);
			return;
		}

		// Connect to server.
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			Connected = false;
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			NotifyError(ErrorType::ConnectionError);
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		std::cerr << "Unable to connect to server!" << std::endl;
		Connected = false;
		WSACleanup();
		NotifyError(ErrorType::ConnectionError);
		return;
	}

	Connected = true;
	NotifyConnectionStatus(true);


	// receive data in background 
	auto workItem = ref new Windows::System::Threading::WorkItemHandler([this](IAsyncAction^ workItem) {
		OnReceive();
		});

	auto asyncAction = Windows::System::Threading::ThreadPool::RunAsync(workItem);
}

bool WeArtClient::IsConnected() {
	return Connected;
}


void WeArtClient::OnReceive() {
	// Buffers
	WSABUF DataBuf;
	char buffer[DATA_BUFSIZE];
	std::string trailingText;


	// Flags and results
	DWORD EventTotal = 0, RecvBytes = 0, Flags = 0;
	DWORD BytesTransferred;

	WSAEVENT EventArray[WSA_MAXIMUM_WAIT_EVENTS];
	EventArray[EventTotal] = WSACreateEvent();
	WSAOVERLAPPED AcceptOverlapped;

	// Receive data
	ZeroMemory(&AcceptOverlapped, sizeof(WSAOVERLAPPED));
	AcceptOverlapped.hEvent = EventArray[EventTotal];
	DataBuf.len = DATA_BUFSIZE;
	DataBuf.buf = buffer;

	EventTotal++;

	if (WSARecv(ConnectSocket, &DataBuf, 1, &RecvBytes, &Flags, &AcceptOverlapped, NULL) == SOCKET_ERROR) {
		if (WSAGetLastError() != WSA_IO_PENDING) {
			std::cerr << "Error reading buffer socket" << std::endl;
			NotifyError(ErrorType::ReceiveMessageError);
		}
	}

	while (TRUE) {
		DWORD Index = WSAWaitForMultipleEvents(EventTotal, EventArray, FALSE, WSA_INFINITE, FALSE);
		WSAResetEvent(EventArray[Index - WSA_WAIT_EVENT_0]);
		WSAGetOverlappedResult(ConnectSocket, &AcceptOverlapped, &BytesTransferred, FALSE, &Flags);

		if (BytesTransferred == 0) {
			std::cout << "Closing socket " << ((int)ConnectSocket) << std::endl;
			closesocket(ConnectSocket);
			WSACloseEvent(EventArray[Index - WSA_WAIT_EVENT_0]);
			NotifyConnectionStatus(false);
			return;
		}

		// Get text to parse and trailing text to use on next receive
		std::string bufferText = std::string(buffer, (size_t)BytesTransferred);
		const int lastSeparatorIndex = bufferText.find_last_of(messagesSeparator);
		std::string textToParse = trailingText + bufferText.substr(0, lastSeparatorIndex);
		trailingText = bufferText.substr(lastSeparatorIndex + 1);

		// Split the string on separator occurence
		std::vector<std::string> splitStrings;
		std::istringstream dataStream(textToParse);
		std::string s;
		while (getline(dataStream, s, messagesSeparator)) {
			if (s.empty())
				continue;
			splitStrings.push_back(s);
		}

		std::vector<WeArtMessage*> messages;

		// Deserialize WeArtMessages
		messages.resize(splitStrings.size());
		for (int i = 0; i < messages.size(); i++) {
			WeArtMessage* msg = messageSerializer.Deserialize(splitStrings[i]);
			if (msg != nullptr)
				messages[i] = msg;
		}

		ForwardingMessages(messages);

		// Receive new data
		Flags = 0;
		ZeroMemory(&AcceptOverlapped, sizeof(WSAOVERLAPPED));
		AcceptOverlapped.hEvent = EventArray[Index - WSA_WAIT_EVENT_0];
		DataBuf.len = DATA_BUFSIZE;
		DataBuf.buf = buffer;
		if (WSARecv(ConnectSocket, &DataBuf, 1, &RecvBytes, &Flags, &AcceptOverlapped, NULL) == SOCKET_ERROR) {
			if (WSAGetLastError() != WSA_IO_PENDING) {
				std::cerr << "Error reading buffer socket" << std::endl;
				NotifyError(ErrorType::ReceiveMessageError);
			}
		}
	}
}

void WeArtClient::Start(TrackingType trackType) {
	StartFromClientMessage startMsg(trackType);
	SendMessage(&startMsg);
}

void WeArtClient::Stop() {
	StopFromClientMessage stopMsg;
	SendMessage(&stopMsg);
}

void WeArtClient::Close() {
	// shutdown the connection since no more data will be sent
	int iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		std::cerr << " Socket shutdown failed with error:" << WSAGetLastErrorString() << std::endl;
		closesocket(ConnectSocket);
		WSACleanup();
		NotifyConnectionStatus(false);
		return;
	}

	// cleanup
	closesocket(ConnectSocket);
	WSACleanup();
	NotifyConnectionStatus(false);
	Connected = false;
}

void WeArtClient::StartCalibration() {
	StartCalibrationMessage startCalibration;
	SendMessage(&startCalibration);
}

void WeArtClient::StopCalibration() {
	StopCalibrationMessage stopCalibration;
	SendMessage(&stopCalibration);
}

void WeArtClient::StartRawData() {
	RawDataOn message;
	SendMessage(&message);
}

void WeArtClient::StopRawData() {
	RawDataOff message;
	SendMessage(&message);
}

void WeArtClient::SendMessage(WeArtMessage* message) {
	if (!Connected)
		return;

	if (message == nullptr)
		return;

	// WeArt message to string
	std::string text = message->serialize();
	text += messagesSeparator;

	char sendbuf[120];
	strcpy_s(sendbuf, text.c_str());

	// Send an initial buffer
	int iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
	if (iResult == SOCKET_ERROR) {
		Connected = false;
		std::cerr << "Send failed with error: " << WSAGetLastErrorString() << std::endl;
		closesocket(ConnectSocket);
		WSACleanup();
		NotifyError(ErrorType::SendMessageError);
		NotifyConnectionStatus(false);
		return;
	}
}

void WeArtClient::ForwardingMessages(std::vector<WeArtMessage*> messages) {
	// Forward the messages to the relevant listeners
	for (auto msg : messages) {
		if (msg == NULL)
			continue;

		for (WeArtMessageListener* listener : messageListeners) {
			if (listener->accept(msg->getID()))
				listener->OnMessageReceived(msg);
		}
	}
}

void WeArtClient::NotifyConnectionStatus(bool connected) {
	for (auto callback : connectionStatusCallbacks)
		pendingCallbacks.emplace_front(std::async(callback, connected));
	ClearProcessedCallbacks();
}

void WeArtClient::NotifyError(ErrorType errorType) {
	for (auto callback : errorCallbacks)
		pendingCallbacks.emplace_front(std::async(callback, errorType));
	ClearProcessedCallbacks();
}

void WeArtClient::ClearProcessedCallbacks() {
	// Remove processed callbacks from vector (garbage collection)
	// Futures must be kept in a list to avoid their premature deletion because of scope
	pendingCallbacks.remove_if([](const std::future<void>& f) {
		return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
		});
}


int WeArtClient::SizeThimbles() {
	return thimbleTrackingObjects.size();
}

void WeArtClient::AddMessageListener(WeArtMessageListener* listener) {
	messageListeners.push_back(listener);
}

void WeArtClient::RemoveMessageListener(WeArtMessageListener* listener) {
	auto it = std::find(messageListeners.begin(), messageListeners.end(), listener);
	if (it != messageListeners.end())
		messageListeners.erase(it);
}

void WeArtClient::AddConnectionStatusCallback(std::function<void(bool)> callback) {
	connectionStatusCallbacks.push_back(callback);
}

void WeArtClient::AddErrorCallback(std::function<void(WeArtClient::ErrorType)> callback) {
	errorCallbacks.push_back(callback);
}

void WeArtClient::AddThimbleTracking(WeArtThimbleTrackingObject* trackingObjects) {
	thimbleTrackingObjects.push_back(trackingObjects);
	AddMessageListener(trackingObjects);
}

void WeArtClient::AddThimbleRawSensors(WeArtRawSensorsData* rawSensorsData) {
	AddMessageListener(rawSensorsData);
}

std::string WSAGetLastErrorString() {
	int errorCode = WSAGetLastError();

	LPWSTR errorBuffer = nullptr;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPWSTR>(&errorBuffer), 0, nullptr);

	std::wstring errorWString(errorBuffer);
	std::string errorString(errorWString.begin(), errorWString.end());

	LocalFree(errorBuffer);
	return errorString;
}

std::string FormatWSAStartupError(int result) {
	switch (result) {
		case WSASYSNOTREADY: return "network subsystem not ready";
		case WSAVERNOTSUPPORTED: return "current winsock version not supported";
		case WSAEINPROGRESS: return "blocking winsock 1.1 operation in progress";
		case WSAEPROCLIM: return "winsock task limit exceeded";
		case WSAEFAULT: return "lpWSAData is not a valid pointer";
		default: return "unknown error (code " + std::to_string(result) + ")";
	}
}