#include "WakeUP.hpp"

WakeUP::WakeUP(short port)
    : acceptor_(io_context_, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)) {
    std::cout << "[WakeUP::Info] Server started on port " << port << std::endl;
    listening();
}
void WakeUP::run() {
    io_context_.run();
}

void WakeUP::listening() {
    
    // Step 01. Initiate asynchronous accept operation.
    
    std::cout << "[WakeUP::Info] Waiting for new connection." << std::endl;
    acceptor_.async_accept(

        // Step 02. Connection accepted handler.
        [this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket) {
            if (!ec) {
                std::cout << "[WakeUP::Info] New connection accepted." << std::endl;

                // Step 03. Create new session and start it.
                auto new_session = std::make_shared<Eys>(std::move(socket));
                
                // Step 04. 
                session_threads_.push_back(std::make_shared<std::thread>([new_session]() {
                    try {
                        new_session->start();
                    } catch (const std::exception& e) {
                        std::cerr << "[WakeUP::Info] Client disconnected."<< std::endl;
                    } catch (...) {
                        std::cerr << "[WakeUP::Error] Unknown exception occurred in session." << std::endl;
                    }
                }));
            } else {
                std::cerr << "[WakeUP::Error] Error accepting new connection: " << ec.message() << std::endl;
            }

            listening();
        });
}

