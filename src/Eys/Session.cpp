#include "Session.hpp"

#include <iostream>
#include <boost/endian/conversion.hpp>
#include <boost/bind.hpp>
#include <chrono>

constexpr uint64_t expected_header = 0x5247424559455345; // "EYESBGR"

Session::Session(boost::asio::ip::tcp::socket socket)
    : socket_(std::move(socket)), evilEys_()
{
    std::cout << "[Session::Info] Session created." << std::endl;
}

Session::~Session() {
    std::cout << "[Session::Info] Session finished." << std::endl;
}

void Session::start() {
    process();
}

int Session::process() {
    // If you want to know all the CV conf (done on compilation time)
    // std::cout << "CV " << cv::getBuildInformation() << std::endl;
    
    // Step 01. Open Sentinel's Eyes (camera)
    if (evilEys_.openEyes() != 0) {
        return -1;
    }

    // Step 02. Define the desired frame rate and alculate the frame interval in milliseconds
    const double frameRate = 8.0; // 4 frames per second
    const auto   frameInterval = std::chrono::milliseconds(static_cast<int>(1000.0 / frameRate));

    while (true) {
        auto startTime = std::chrono::steady_clock::now(); // Mark the start time

        // Step 03. Get the frame from Sentinel's Eye
        cv::Mat frame = evilEys_.spark();

        if (frame.empty()) {
            std::cerr << "[Session::Info] Frame is empty!" << std::endl;
            break;
        }

        // Step 04. Send the frame to client
        send_frame(frame);

        // Calculate how long it took to capture and send the frame
        auto endTime = std::chrono::steady_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

        // Step 05. If it took less time than the desired frame interval, sleep for the remaining time
        // It garantees the FPS
        if (elapsedTime < frameInterval) {
            std::this_thread::sleep_for(frameInterval - elapsedTime);
        }
    }
    return 0;
}

void Session::send_frame(const cv::Mat& frame) {
    // The send_frame function takes a reference to the socket and a reference to the image to be sent

    // Whenever a connection is established, we need to send a header to the client
    // indicating that the communication is starting. This is done so that the client knows
    // that the connection is established and it can start receiving data.
    // The header is a fixed value known by both sides of the connection.
    // To facilitate implementation, we'll use the fixed and sequential 64-bit value below:
    uint64_t header = expected_header;
    boost::asio::write(socket_, boost::asio::buffer(&header, sizeof(header)));

    // Whenever we send data to the client, we need to do a few things:
    // 1. Have a suitable buffer to store the data to be sent
    // 2. Convert the value to network byte order
    // 3. Send the buffer size to the client before sending the data
    // 4. Send the data
    // Let's follow these steps

    // Step 1. Have a suitable buffer to store the data to be sent
    // The buffer vector is the frame, but converted to jpg
    std::vector<uchar> buffer;
    cv::imencode(".jpg", frame, buffer);

    // Step 2. Convert the value to network byte order
    // This is necessary because the network byte order is different from the byte order of the computer.
    // Failing to convert the value to network byte order can cause communication errors.
    std::size_t bufferSize = buffer.size();
    uint64_t bufferSize_network = be64toh(bufferSize);

    // Step 3. Send the buffer size to the client before sending the data
    // This is necessary because the client needs to know how many bytes it should expect to receive
    // before attempting to read the data. In other words, it knows when the communication starts and when it ends.
    boost::asio::write(socket_, boost::asio::buffer(&bufferSize_network, sizeof(bufferSize_network)));

    // Step 4. Send the image data
    boost::asio::write(socket_, boost::asio::buffer(buffer.data(), bufferSize));
}
