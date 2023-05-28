/**
 * @file   WakeUP.hpp
 * @brief  The WakeUP Server Header File.
 *
 * This file is part of the Evil Eyes project:
 *   - Website: http://www.aincrivelfabrica.com.br
 *   - GitHub: https://github.com/felipedepauli/evil-eyes
 *
 * License:
 *     Copyright (C) 2023 Felipe Camargo de Pauli
 *     All Rights Reserved.
 *
 *     This source code is provided for viewing purposes only. No part of this source code
 *     may be used, modified, distributed, or sublicensed for any purpose without the
 *     express written consent of the copyright holder.
 */

#include <boost/asio.hpp>
#include <memory>
#include <thread>
#include <vector>
#include "Session.hpp"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <chrono>
#include <fstream>

using boost::asio::ip::tcp;

/**
 * @class WakeUP
 * @brief Handle new connections.
 */ 
class WakeUP {
public:
    /**
     * @brief Constructor for the WakeUP server.
     * @param port The port on which the server will run.
     */
    WakeUP(short port);
    
    /**
     * @brief Runs the server, starting the event loop for incoming connections.
     */
    void run();

private:
    /**
     * @brief Starts an asynchronous operation to accept a new connection.
     */
    void listening();

    boost::asio::io_context io_context_;                        /// Keeps the context of boost. It has the context and the object used to accept new connections
    tcp::acceptor acceptor_;                                    /// Object to accept new connections
    std::vector<std::shared_ptr<std::thread>> session_threads_; /// All new session will be here
};