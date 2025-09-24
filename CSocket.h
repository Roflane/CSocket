#pragma once
#include <WinSock2.h>
#include <ws2tcpip.h>

#pragma comment (lib, "Ws2_32.lib")


class CSocket {
private:
    WSADATA _wsaData;
    SOCKET _serverSocket = INVALID_SOCKET;
    addrinfo _hints{};
    addrinfo* _outAddrInfo = nullptr;
public:
    explicit CSocket(const char *ip, const char *port, const int sock_type, const int protocol) {
        WSAStartup(MAKEWORD(2,2), &_wsaData);
        ZeroMemory(&_hints, sizeof(_hints));

        _hints.ai_family = AF_INET;
        _hints.ai_socktype = sock_type;
        _hints.ai_protocol = protocol;

        getaddrinfo(ip, port, &_hints, &_outAddrInfo);
        _serverSocket = socket(_hints.ai_family, _hints.ai_socktype, _hints.ai_protocol);
    }
    ~CSocket() { Close(); }

    // Properties
    SOCKET getSocket() const { return _serverSocket; }
    addrinfo* getAddrInfo() const { return _outAddrInfo; }
    WSADATA getWSAData() const { return _wsaData; }

    // Methods
    [[nodiscard]] int Bind() {
        _serverSocket = socket(_hints.ai_family, _hints.ai_socktype, _hints.ai_protocol);
        return bind(_serverSocket, _outAddrInfo->ai_addr, sizeof(addrinfo));
    }

     [[nodiscard]] int Listen() const {
        return listen(_serverSocket, SOMAXCONN);
    }

    [[nodiscard]] int Connect() const {
        return connect(_serverSocket, _outAddrInfo->ai_addr, static_cast<int>(_outAddrInfo->ai_addrlen));
    }

    [[nodiscard]] SOCKET Accept() const {
        return accept(_serverSocket, _outAddrInfo->ai_addr, reinterpret_cast<int*>(_outAddrInfo->ai_addrlen));
    }

    int Send(const char *buffer, const int length) const {
        return send(_serverSocket, buffer,  length, NULL);
    }

    int Receive(char *buffer, const int length) const {
        return recv(_serverSocket, buffer, length, NULL);
    }

    [[nodiscard]] int Disconnect() const {
        return shutdown(_serverSocket, SD_BOTH);
    }

    void Close() const {
        freeaddrinfo(_outAddrInfo);
        closesocket(_serverSocket);
        WSACleanup();
    }
};