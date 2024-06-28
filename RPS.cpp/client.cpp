#include <iostream>
#include <boost/asio.hpp>
#include "game.h"

using boost::asio::ip::tcp;

const std::string host = "127.0.0.1";
const int port = 12345;

void playGame(tcp::socket& socket) {
    boost::system::error_code error;

    while (true) {
        std::string move;
        std::cout << "Enter your move (rock, paper, scissors): ";
        std::cin >> move;

        boost::asio::write(socket, boost::asio::buffer(move), error);

        if (error) {
            throw boost::system::system_error(error);
        }

        char reply[512];
        size_t reply_length = socket.read_some(boost::asio::buffer(reply), error);
        if (error == boost::asio::error::eof)
            break; // Connection closed cleanly by peer.
        else if (error)
            throw boost::system::system_error(error); // Some other error.

        std::cout << "Server reply: " << std::string(reply, reply_length) << std::endl;
    }
}

int main() {
    try {
        boost::asio::io_context io_context;
        tcp::resolver resolver(io_context);
        tcp::resolver::results_type endpoints = resolver.resolve(host, std::to_string(port));
        tcp::socket socket(io_context);

        boost::asio::connect(socket, endpoints);

        std::cout << "Connected to server." << std::endl;
        playGame(socket);
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
