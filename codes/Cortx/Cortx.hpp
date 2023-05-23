#ifndef CORTX_H
#define CORTX_H

#include <string>
#include <boost/asio.hpp>
#include <opencv2/opencv.hpp>
#include "Processing/Detector.hpp"

using boost::asio::ip::tcp;

/**
 * @file    Cortx.hpp
 * @brief   Header file for the Cortx class.
 * @details This class provides the main functionality of the Cortex program. It handles the network connection and the detector.
 */

/**
 * @class   Cortx
 * @brief   This class provides the main functionality of the Cortex program.
 * @details The Cortx class is the main class of the program. It handles the network connection and the detector.
 */
class Cortx {
public:
    /**
     * @brief Constructor for the Cortx class.
     * @param host The IP address of the server.
     * @param port The port number of the server.
     */
    Cortx(const std::string& host, unsigned short port);
    
    /**
     * @brief Main method of the Cortx class. It starts the network connection and runs the detector.
     */
    void run();

private:
    
    /// @brief The IP address of the server.
    const std::string host_;
    /// @brief The port number of the server.
    unsigned short port_;
    /// @brief The Detector object, which handles the detection functionality.
    Detector detector_;

    /**
     * @brief This method sets up the network connection.
     * @param socket A reference to the socket object to be connected.
     */
    void connection(tcp::socket& socket);
};

#endif // CORTX_H
