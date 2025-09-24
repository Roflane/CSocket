#include <iostream>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <thread>
#include <vector>
#include "CSocket.h"

#pragma comment (lib, "Ws2_32.lib")

#define log_red(fmt, ...) printf("\033[31m" fmt "\033[0m\n", ##__VA_ARGS__)
#define log_green(fmt, ...) printf("\033[32m" fmt "\033[0m\n", ##__VA_ARGS__)
#define log_blue(fmt, ...) printf("\033[34m" fmt "\033[0m\n", ##__VA_ARGS__)

#define BUFFER_CAP 1024

int main() {
    CSocket sock("4773", SOCK_STREAM, IPPROTO_TCP, true);

    SOCKET serverSocket = sock.getSocket();

    if (serverSocket == INVALID_SOCKET) {
        log_red("Failed to create server socket");
        return -1;
    }

    if (const int result = sock.Bind(); result != 0) {
        log_red("Failed to bind server socket: %d", result);
        return result;
    }

    if (const int result = sock.Listen(); result != 0) {
        log_red("Failed to listen on socket: %d", result);
        return result;
    }

    std::vector<SOCKET> clients;
    std::vector<std::thread> clientThreads;

    std::thread mainThread {[&]() {
            while (true) {
                log_blue("Waiting for a client...");

                SOCKET clientSocket = sock.Accept(nullptr, nullptr);
                if (clientSocket == INVALID_SOCKET) {
                    log_red("Invalid client socket!");
                    break;
                }

                log_green("\nClient accepted!");

                std::thread clientThread ([&, clientSocket](){
                        try {
                            std::string buffer = {};
                            buffer.resize(BUFFER_CAP);

                            const int nicknameData = sock.ReceiveFrom(clientSocket, buffer.data(), BUFFER_CAP);
                            if (nicknameData > 0) {
                                const std::string nickname = buffer.substr(0, nicknameData);
                                log_green("Established connection with %s", nickname.c_str());

                                while (true) {
                                    const int messageData = sock.ReceiveFrom(clientSocket, buffer.data(), BUFFER_CAP);

                                    if (messageData > 0) {
                                        const std::string message = buffer.substr(0, messageData);
                                        log_green("Message from %s: %s", nickname.c_str(), message.c_str());
                                    }
                                    else {
                                        CSocket::CloseClient(clientSocket);
                                        log_red("Client '%s' has been disconnected!", nickname.c_str());
                                        break;
                                    }
                                }
                            }
                        }
                    catch (const std::exception &e) {
                        CSocket::CloseClient(clientSocket);
                        log_red("Exception: %s", e.what());
                        log_red("Client disconnected!");
                    }
                });

                clients.push_back(clientSocket);
                clientThreads.push_back(std::move(clientThread));
            }
        }
    };

    mainThread.join();

    for (const auto &client : clients) {
        CSocket::CloseClient(client);
    }

    for (auto& thread : clientThreads) {
        thread.join();
    }
}