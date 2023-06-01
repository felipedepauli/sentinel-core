/**
 * @file   Session.cpp
 * @brief  The Session Source File.
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

#include "Session.hpp"

// Constructor for the Session class
// Initializes the socket and the evilEys_ object
Session::Session(boost::asio::ip::tcp::socket socket)
    : socket_(std::move(socket)), evilEys_()
{
    // Log the creation of a new session
    std::cout << "[Session::Info] Session created." << std::endl;
}

// Destructor for the Session class
Session::~Session() {
    // Log the end of a session
    std::cout << "[Session::Info] Session finished." << std::endl;
}

// Starts the session by calling the process function
void Session::start() {
    process();
}

// Handles the processing of client requests
int Session::process() {
    // Open the camera
    if (evilEys_.openEyes() != 0) {
        return -1;
    }

    // Define the frame rate and calculate the frame interval
    const double frameRate = 4.0; // 4 frames per second
    const auto   frameInterval = std::chrono::milliseconds(static_cast<int>(1000.0 / frameRate));

    // Loop to handle client requests
    while (true) {
        auto startTime = std::chrono::steady_clock::now(); // Mark the start time

        // Get the frame from the camera
        cv::Mat frame = evilEys_.spark();

        // Check if the frame is empty
        if (frame.empty()) {
            std::cerr << "[Session::Info] Frame is empty!" << std::endl;
            break;
        }

        // Send the frame to the client
        send_frame(frame);

        // Calculate the elapsed time
        auto endTime = std::chrono::steady_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

        // If the elapsed time is less than the frame interval, sleep for the remaining time
        if (elapsedTime < frameInterval) {
            std::this_thread::sleep_for(frameInterval - elapsedTime);
        }
    }
    return 0;
}

// Sends a frame to the client
void Session::send_frame(const cv::Mat& frame) {
    // Send the header to the client
    uint64_t header = expected_header;
    boost::asio::write(socket_, boost::asio::buffer(&header, sizeof(header)));

    // Convert the frame to jpg and store it in a buffer
    std::vector<uchar> buffer;
    cv::imencode(".jpg", frame, buffer);

    // Convert the buffer size to network byte order
    std::size_t bufferSize = buffer.size();
    uint64_t bufferSize_network = be64toh(bufferSize);

    // Send the buffer size to the client
    boost::asio::write(socket_, boost::asio::buffer(&bufferSize_network, sizeof(bufferSize_network)));

    // Send the image data to the client
    boost::asio::write(socket_, boost::asio::buffer(buffer.data(), bufferSize));
}
