/**
 * @file   Eys.hpp
 * @brief  The Eys Server Header File.
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
 * @class Eys
 * @brief Main class for the Eys server.
 */
class Eys {
public:
    /**
     * @brief Constructor for the Eys server.
     *
     * @param port The port on which the server will run.
     */
    Eys(short port);
    
    /**
    /**
     * @brief Runs the server, starting the event loop for incoming connections.
     */
    void run();

private:
    /**
     * @brief Starts an asynchronous operation to accept a new connection.
     */
    void do_accept();

    boost::asio::io_context io_context_;
    tcp::acceptor acceptor_;
    std::vector<std::shared_ptr<std::thread>> session_threads_;
};