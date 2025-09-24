#include <cstdio>
#include <iostream>
#include <iterator>
#include "CSocket.h"

#define LOG(x, ...) printf(x "\n", ##__VA_ARGS__)

int main() {
    const char *ip = "192.168.31.121";
    const char *port = "4773";
    int socket_type = SOCK_STREAM;
    int protocol = IPPROTO_TCP;

    CSocket sock(ip, port, socket_type, protocol);

    if (sock.getSocket() == INVALID_SOCKET) {
        printf("Socket creation failed: %d\n", WSAGetLastError());
    }
    if (sock.Connect() == SOCKET_ERROR) {
        printf("Connect failed: %d\n", WSAGetLastError());
    }

    LOG("Input your nickname: ");
    std::string nicknameBuffer;
    std::cin >> nicknameBuffer;
    const int bytesSent = sock.Send(nicknameBuffer.c_str(), nicknameBuffer.size());
    LOG("bytes sent: %i\n", bytesSent);
    LOG("Successfully authorized.");

    while (true) {
        std::string msgBuffer;
        std::getline(std::cin, msgBuffer);
        const int bytesSent2 = sock.Send(msgBuffer.c_str(), msgBuffer.size());
        LOG("bytes sent: %i\n", bytesSent2);
    }
}