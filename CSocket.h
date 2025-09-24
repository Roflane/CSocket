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
    bool _isServer = false;
public:
    explicit CSocket(const char *port, const int sock_type, const int protocol, const bool isServer = true)
        : _isServer(isServer) {
        WSAStartup(MAKEWORD(2,2), &_wsaData);
        ZeroMemory(&_hints, sizeof(_hints));

        _hints.ai_family = AF_INET;
        _hints.ai_socktype = sock_type;
        _hints.ai_protocol = protocol;

        if (_isServer) {
            _hints.ai_flags = AI_PASSIVE;
            getaddrinfo(nullptr, port, &_hints, &_outAddrInfo);
        }
        _serverSocket = CreateSocket();
    }

    explicit CSocket(const char *ip, const char *port, const int sock_type, const int protocol)
        : _isServer(false) {
        WSAStartup(MAKEWORD(2,2), &_wsaData);
        ZeroMemory(&_hints, sizeof(_hints));

        _hints.ai_family = AF_INET;
        _hints.ai_socktype = sock_type;
        _hints.ai_protocol = protocol;

        getaddrinfo(ip, port, &_hints, &_outAddrInfo);
        _serverSocket = CreateSocket();
    }

    ~CSocket() { Close(); }

    // Properties
    SOCKET getSocket() const { return _serverSocket; }
    addrinfo* getAddrInfo() const { return _outAddrInfo; }
    WSADATA getWSAData() const { return _wsaData; }
    bool isServer() const { return _isServer; }

    // Methods
    [[nodiscard]] SOCKET CreateSocket() {
        _serverSocket = socket(_outAddrInfo->ai_family, _outAddrInfo->ai_socktype, _outAddrInfo->ai_protocol);
        return _serverSocket;
    }

    [[nodiscard]] int Bind() const {
        return bind(_serverSocket, _outAddrInfo->ai_addr, static_cast<int>(_outAddrInfo->ai_addrlen));
    }

    [[nodiscard]] int Listen(int backlog = SOMAXCONN) const {
        return listen(_serverSocket, backlog);
    }

    [[nodiscard]] int Connect() const {
        return connect(_serverSocket, _outAddrInfo->ai_addr, static_cast<int>(_outAddrInfo->ai_addrlen));
    }

    [[nodiscard]] SOCKET Accept(sockaddr *addr = nullptr, socklen_t *addrlen = nullptr) const {
        return accept(_serverSocket, addr, addrlen);
    }

    int Send(const char *buffer, const int length, const int flags = 0) const {
        return send(_serverSocket, buffer, length, flags);
    }

    int Receive(char *buffer, const int length, const int flags = 0) const {
        return recv(_serverSocket, buffer, length, flags);
    }

    int SendTo(const SOCKET clientSocket, const char *buffer, const int length, const int flags = 0) const {
        return send(clientSocket, buffer, length, flags);
    }

    int ReceiveFrom(const SOCKET clientSocket, char *buffer, const int length, const int flags = 0) const {
        return recv(clientSocket, buffer, length, flags);
    }

    [[nodiscard]] int Disconnect() const {
        return shutdown(_serverSocket, SD_BOTH);
    }

    void Close() {
        if (_serverSocket != INVALID_SOCKET) {
            closesocket(_serverSocket);
            _serverSocket = INVALID_SOCKET;
        }
        if (_outAddrInfo) {
            freeaddrinfo(_outAddrInfo);
            _outAddrInfo = nullptr;
        }
        WSACleanup();
    }

    static int CloseClient(const SOCKET clientSocket) {
        if (clientSocket == INVALID_SOCKET) {
            return SOCKET_ERROR;
        }
        return closesocket(clientSocket);
    }
};;