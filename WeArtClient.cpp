/**
*	WEART - Common TCP Client
*	https://www.weart.it/
*/

#define WIN32_LEAN_AND_MEAN

#include "pch.h"
#include <WEART_SDK/WeArtClient.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <future>

#include <ppltasks.h>
using namespace Windows::Foundation;
using namespace concurrency;
using namespace Windows::Devices::Enumeration;

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_BUFLEN 512
int recvbuflen = DEFAULT_BUFLEN;
char recvbuf[DEFAULT_BUFLEN];

#define DATA_BUFSIZE 4096

static char messagesSeparator = '~';

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
        printf("WSAStartup failed with error: %d\n", iResult);
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
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        NotifyError(ErrorType::ConnectionError);
        return;
    }

    // Attempt to connect to an address until one succeeds
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            Connected = false;
            printf("socket failed with error: %ld\n", WSAGetLastError());
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
        printf("Unable to connect to server!\n");
        Connected = false;
        WSACleanup();
        NotifyError(ErrorType::ConnectionError);
        return;
    }

    Connected = true;
    NotifyConnectionStatus(true);
    
    
    // receive data in background 
    auto workItem = ref new Windows::System::Threading::WorkItemHandler([this](IAsyncAction^ workItem)
        {
            OnReceive();
        });

    auto asyncAction = Windows::System::Threading::ThreadPool::RunAsync(workItem);
}

bool WeArtClient::IsConnected() {
    return Connected;
}


void WeArtClient::OnReceive() {

    WSABUF DataBuf;

    char buffer[DATA_BUFSIZE];

    DWORD EventTotal = 0, RecvBytes = 0, Flags = 0;
    DWORD BytesTransferred;

    WSAEVENT EventArray[WSA_MAXIMUM_WAIT_EVENTS];

    WSAOVERLAPPED AcceptOverlapped;

    std::string trailingText;

    EventArray[EventTotal] = WSACreateEvent();

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

    while (TRUE)
    {
        DWORD Index;

        Index = WSAWaitForMultipleEvents(EventTotal, EventArray, FALSE, WSA_INFINITE, FALSE);

        WSAResetEvent(EventArray[Index - WSA_WAIT_EVENT_0]);

        WSAGetOverlappedResult(ConnectSocket, &AcceptOverlapped, &BytesTransferred, FALSE, &Flags);

        if (BytesTransferred == 0)
        {
            printf("Closing socket %d\n", ConnectSocket);
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

        // Std::strings to WeArtMessages
        messages.resize(splitStrings.size());
        for (int i = 0; i < messages.size(); i++) {
            messages[i] = messageSerializer.Deserialize(splitStrings[i]);
        }

        ForwardingMessages(messages);

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

    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

    // shutdown the connection since no more data will be sent
    int iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
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

void WeArtClient::SendMessage(WeArtMessage* message) {
    if (!Connected) {
        return;
    }

    // WeArt message to string
    std::string text = messageSerializer.Serialize(message);
    text += messagesSeparator;

    char sendbuf[120];
    strcpy_s(sendbuf, text.c_str());

    // Send an initial buffer
    int iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
    if (iResult == SOCKET_ERROR) {
        Connected = false;
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        NotifyError(ErrorType::SendMessageError);
        NotifyConnectionStatus(false);
        return;
    }
}

void WeArtClient::ForwardingMessages(std::vector<WeArtMessage*> messages)
{
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

void WeArtClient::AddThimbleTracking(WeArtThimbleTrackingObject* trackingObjects) {
    thimbleTrackingObjects.push_back(trackingObjects);
    AddMessageListener(trackingObjects);
}

void WeArtClient::AddThimbleRawSensors(WeArtRawSensorsData * rawSensorsData) {
    AddMessageListener(rawSensorsData);
}
