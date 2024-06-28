#include <iostream>
#include <boost/asio.hpp>
#include "game.h"

using boost::asio::ip::tcp;

const int port = 12345;

void handleClient(tcp::socket& socket) {
    boost::system::error_code error;
    Game game(5);

    while (!game.isGameOver()) {
        char data[512];
        size_t length = socket.read_some(boost::asio::buffer(data), error);
        if (error == boost::asio::error::eof)
            break; // Connection closed cleanly by peer.
        else if (error)
            throw boost::system::system_error(error); // Some other error.

        std::string move1(data, length);
        std::string move2;
        std::cout << "Enter server's move (rock, paper, scissors): ";
        std::cin >> move2;

        game.playRound(game.getMoveFromString(move1), game.getMoveFromString(move2));

        std::string result = game.getResult();
        boost::asio::write(socket, boost::asio::buffer(result), error);
    }

    game.displayStats();
}

int main() {
    try {
        boost::asio::io_context io_context;

        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), port));

        std::cout << "Server is running on port " << port << std::endl;

        while (true) {
            tcp::socket socket(io_context);
            acceptor.accept(socket);

            std::cout << "Client connected." << std::endl;
            handleClient(socket);
            std::cout << "Client disconnected." << std::endl;
        }
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
