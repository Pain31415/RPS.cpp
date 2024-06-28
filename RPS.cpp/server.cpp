#include <iostream>
#include <string>
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <chrono>
#include "Game.h"

#pragma comment(lib, "Ws2_32.lib")

void playGame(SOCKET clientSocket, Game::GameMode mode, int rounds) {
    char buffer[1024] = { 0 };
    Game game(rounds, mode);

    while (!game.isGameOver()) {
        Move player1Move = Move::INVALID;
        Move player2Move = Move::INVALID;

        std::cout << "Enter your move (rock, paper, scissors): ";
        std::string moveStr;
        std::cin >> moveStr;
        player1Move = game.getMoveFromString(moveStr);

        if (send(clientSocket, moveStr.c_str(), moveStr.length(), 0) == SOCKET_ERROR) {
            std::cerr << "Send failed: " << WSAGetLastError() << std::endl;
            break;
        }

        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived > 0) {
            player2Move = game.getMoveFromString(std::string(buffer, bytesReceived));
            std::cout << "Player 2 move: " << game.moveToString(player2Move) << std::endl;
            game.playRound(player1Move, player2Move);
        }
        else if (bytesReceived == 0) {
            std::cout << "Connection closed." << std::endl;
            break;
        }
        else {
            std::cerr << "Recv failed: " << WSAGetLastError() << std::endl;
            break;
        }
    }

    game.displayStats();
    closesocket(clientSocket);
}

int main() {
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        std::cerr << "WSAStartup failed: " << iResult << std::endl;
        return 1;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(5000); // «м≥н≥ть на бажаний порт
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        std::cerr << "Bind failed: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    if (listen(serverSocket, 5) == SOCKET_ERROR) {
        std::cerr << "Listen failed: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Waiting for client connection..." << std::endl;

    SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Accept failed: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Client connected." << std::endl;

    int rounds = 5;
    int modeChoice;
    std::cout << "Enter number of rounds per game: ";
    std::cin >> rounds;

    std::cout << "Choose game mode:\n";
    std::cout << "1. Player vs Player\n";
    std::cout << "2. Player vs Computer\n";
    std::cout << "3. Computer vs Computer\n";
    std::cout << "Enter mode choice: ";
    std::cin >> modeChoice;

    Game::GameMode mode;
    switch (modeChoice) {
    case 1:
        mode = Game::GameMode::PLAYER_VS_PLAYER;
        break;
    case 2:
        mode = Game::GameMode::PLAYER_VS_COMPUTER;
        break;
    case 3:
        mode = Game::GameMode::COMPUTER_VS_COMPUTER;
        break;
    default:
        std::cerr << "Invalid choice. Defaulting to Player vs Player mode.\n";
        mode = Game::GameMode::PLAYER_VS_PLAYER;
        break;
    }

    playGame(clientSocket, mode, rounds);

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
