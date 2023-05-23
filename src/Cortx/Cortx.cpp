/**
 * @file   Cortx.cpp
 * @brief  Implementation of the Cortx class.
 * @details The Cortx class has three important attributes for the program's operation:
 *              - The IP address of the Eys server;
 *              - The port used by the Eys server;
 *              - The detector, an object of the Detector class, responsible for processing object detection (Computer Vision pipeline).
 * To establish the TCP/IP connection with the server, the boost.beast library is used.
 */

#include "Cortx.hpp"
#include <iostream>
#include <boost/asio.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <fstream>
#include <iomanip>

// Definition that tcp used is boost::asio::ip::tcp
using boost::asio::ip::tcp;

// Implementation of the constructor
Cortx::Cortx(const std::string& host, unsigned short port)
    : host_(host), port_(port), detector_("coco.names", "yolov4-tiny.cfg", "yolov4-tiny.weights") {
    // Nothing more needed here
}

// Main run method for the class
void Cortx::run() {
    // Step 01. A context is created. A context stores information about the state of the program.
    boost::asio::io_context io_service;

    try {
        // Step 02. Creating endpoint to server
        std::cout << "[Cortx] Creating endpoint to server..." << std::endl;
        tcp::endpoint endpoint(boost::asio::ip::address::from_string(host_), port_);

        // Step 03. Creating and opening an active socket
        tcp::socket socket(io_service, endpoint.protocol());
        std::cout << "[Cortx] Socket created and opened!" << std::endl;

        // Step 04. Connecting to the server
        socket.connect(endpoint);

        std::cout << "[Cortx] Connected to server ip: " << host_ << ":" << port_ << std::endl;

        // Step 05. After connection, start the main functionality
        connection(socket);

    } catch (const boost::system::system_error& err) {
        // Report any errors during the connection process
        std::cerr << "[Cortx] Error occurred! Error code = " << err.code() << ". Message: " << err.what() << std::endl;
    }
}

// Implementation of the connection method
void Cortx::connection(tcp::socket& socket) {
    std::cout << "[Cortx] Starting frame receiving..." << std::endl;

    // Step 01. Open the file for binary output
    std::ofstream fifo("/tmp/synapse", std::ios::binary);

    if (!fifo.is_open()) {
        // Report an error if the file does not open
        std::cerr << "Error when opening the FIFO file." << std::endl;
        return;
    }

    // Try to process the frames received
    try {
        // Step 02. Keep track of the number of frames
        int frame_number = 0;

        // Continue processing as long as frames are being received
        while (true) {

            // Step 03. Read the header
            uint64_t header = 0;
            boost::asio::read(socket, boost::asio::buffer(&header, sizeof(header)));

            // Step 04. Check the validity of the header
            if (header != 0x123456789ABCDEF0) {
                // If the header is not valid, break the loop
                break;
            }

            // Step 05. Receive the buffer size
            uint64_t bufferSize_network;
            boost::asio::read(socket, boost::asio::buffer(&bufferSize_network, sizeof(bufferSize_network)));
            std::size_t bufferSize = be64toh(bufferSize_network);

            // Step 06. Receive the image data
            std::vector<uchar> buffer(bufferSize);
            boost::asio::read(socket, boost::asio::buffer(buffer.data(), bufferSize));

            // Step 07. Decode the image
            cv::Mat frame = cv::imdecode(buffer, cv::IMREAD_COLOR);
            if (!frame.empty()) {

                // Step 08. Detect the objects in the frame
                cv::Mat annotated_frame = detector_.annotate_frame(frame);

                if (!annotated_frame.empty()) {
                    // Step 09. Encode the annotated frame as a JPG
                    std::vector<uchar> buf;
                    cv::imencode(".jpg", annotated_frame, buf);

                    // Step 10. Sent the result to the destination
                    fifo.write(reinterpret_cast<char*>(buf.data()), buf.size());

                    // Step 11. Flush the buffer
                    fifo.flush();
                }
            } else {
                std::cerr << "[Cortx] Received empty frame. Skipping..." << std::endl;
            }

            // Step 12. Keep count of the number of frames processed
            frame_number++;
            if (frame_number % 100 == 0) {
                std::cout << "[Cortx] Processed " << frame_number << " frames..." << std::endl;
            }
        }
    } catch (const boost::system::system_error& err) {
        // Report any errors during the frame processing
        std::cerr << "[Cortx] Error occurred! Error code = " << err.code() << ". Message: " << err.what() << std::endl;
    }

    // Step 13. Close the file when done
    fifo.close();
}

