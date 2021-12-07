#define WIN32_LEAN_AND_MEAN

#include "pch.h"
#include <WEART_SDK/WeArtClient.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>ù

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
WeArtClient::WeArtClient(PCSTR IP_ADDESS, PCSTR PORT) {

    WSADATA wsaData;
    ConnectSocket = INVALID_SOCKET;
    struct addrinfo* result = NULL,
        * ptr = NULL,
        hints;

    int iResult;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        IsConnected = false;
        printf("WSAStartup failed with error: %d\n", iResult);
        return;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo(IP_ADDESS, PORT, &hints, &result);
    if (iResult != 0) {
        IsConnected = false;
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return;
    }

    // Attempt to connect to an address until one succeeds
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            IsConnected = false;
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return;
        }

        // Connect to server.
        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            IsConnected = false;
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        IsConnected = false;
        WSACleanup();
        return;
    }

    IsConnected = true;

    //ReadData();
    OnReceive();
}


void WeArtClient::OnReceive()
{

    DWORD RecvBytes, Flags;
    WSABUF DataBuf;

    std::string bufferText;
    bufferText.resize(1024);
    
    DataBuf.len = 1024;
    DataBuf.buf = &bufferText[0];

    std::string trailingText;

    int err = 0;

    WSAOVERLAPPED RecvOverlapped;

    // Make sure the RecvOverlapped struct is zeroed out
    SecureZeroMemory((PVOID)&RecvOverlapped, sizeof(WSAOVERLAPPED));

    struct addrinfo* result = NULL;

    // Call WSARecv until the peer closes the connection
    // or until an error occurs
    while (1) {

        Flags = 0;
        int rc = WSARecv(ConnectSocket, &DataBuf, 1, &RecvBytes, &Flags, &RecvOverlapped, NULL);
        if ((rc == SOCKET_ERROR) && (WSA_IO_PENDING != (err = WSAGetLastError()))) {
            printf("WSARecv failed with error: %d\n", err);
            break;
        }

        const int lastSeparatorIndex = bufferText.find_last_of(messagesSeparator);
        trailingText = bufferText.substr(lastSeparatorIndex + 1);
        std::string textToParse = bufferText.substr(0, lastSeparatorIndex);

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

        
        for (auto msg : messages) {

            if (msg == NULL)
                continue;

           
            // Forward the message to relevant tracking objects
            for (WeArtThimbleTrackingObject* obj : thimbleTrackingObjects) {
                obj->OnMessageReceived(msg);
            }
        }

        WSAResetEvent(RecvOverlapped.hEvent);

        // If 0 bytes are received, the connection was closed
        if (RecvBytes == 0)
            break;
    }

}

void WeArtClient::Start() {
    StartFromClientMessage startMsg;
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
        return;
    }

    // cleanup
    closesocket(ConnectSocket);
    WSACleanup();

    IsConnected = false;
}

void WeArtClient::SendMessage(WeArtMessage* message) {

    if (!IsConnected) {
        return;
    }

    // WeArt message to string
    std::string text = messageSerializer.Serialize(message);
    text += messagesSeparator;

    char sendbuf[120];
    strcpy(sendbuf, text.c_str());

    // Send an initial buffer
    int iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
    if (iResult == SOCKET_ERROR) {
        IsConnected = false;
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return;
    }
}


void WeArtClient::AddThimbleTracking(WeArtThimbleTrackingObject* trackingObjects) {

    this->thimbleTrackingObjects.push_back(trackingObjects);
}
