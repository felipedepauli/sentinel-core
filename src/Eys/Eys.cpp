#include "Eys.hpp"

Eys::Eys(short port)
    : acceptor_(io_context_, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)) {
    do_accept();
}
void Eys::run() {
    io_context_.run();
}

void Eys::do_accept() {
    
    // Initiate asynchronous accept operation.
    std::cout << "[Eys::Info] Waiting for new connection." << std::endl;
    acceptor_.async_accept(
        // Connection accepted handler.
        [this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket) {
            if (!ec) {
                std::cout << "[Eys::Info] New connection accepted." << std::endl;

                // Create new session and start it.
                auto new_session = std::make_shared<Session>(std::move(socket));
                
                std::cout << "[Eys::Info] Starting new session." << std::endl;
                session_threads_.push_back(std::make_shared<std::thread>([new_session]() {
                    try {
                        new_session->start();
                    } catch (const std::exception& e) {
                        std::cerr << "[Eys::Info] Client disconnected."<< std::endl;
                    } catch (...) {
                        std::cerr << "[Eys::Error] Unknown exception occurred in session." << std::endl;
                    }
                }));
            } else {
                std::cerr << "[Eys::Error] Error accepting new connection: " << ec.message() << std::endl;
            }

            do_accept();
        });
}

