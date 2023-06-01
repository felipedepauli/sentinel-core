/**
 * @file   main.cpp
 * @brief  The Cortx's Main File.
 * @details This is the main entry point of the Cortex program. It processes the command line arguments, 
 *          sets up signal handling, and runs the Cortex class. The Cortex class is responsible for the main logic of the program.
 */

#include <iostream>     // For standard I/O operations
#include "Cortx.hpp"    // Include the Cortx class
#include <csignal>      // For handling Linux signals

// Function to handle the SIGINT signal (Ctrl+C)
void handleSignal(int signal) {
    if (signal == SIGINT) {
        std::cout << " The Cortex Program was -> "  << signal << std::endl;
        exit(signal);
    }
}

/**
 * @brief The entry point of the program. The only objective here is to get the port number, instantiate the class Cortx, to keep it running and handle the signals.
 * @details This function is responsible for the initial setup of the Cortex program. It verifies the command-line arguments, 
 *          handles the system signals, instantiates the Cortx class, and runs the main loop of the program.
 * @param argc The count of the command-line arguments.
 * @param argv The array of the command-line arguments. The First argument is the IP and the second is the Port of the server.
 * @return 0 if the program ends successfully, 1 if the command-line arguments are incorrect.
 */
int main(int argc, char* argv[]) {

    // Check if the correct number of command-line arguments were passed
    if (argc != 3) {
        std::cerr << "Usage: client <ip> <port>\n";
        return 1;
    }

    // Get the IP and port from the command-line arguments
    std::string ip = argv[1];
    unsigned short port = std::atoi(argv[2]);

    // Set up the signal handler for SIGINT
    signal(SIGINT, handleSignal);

    // Instantiate the Cortx class
    Cortx cortex(ip, port);

    // Run the main loop of the Cortx class
    cortex.run();

    return 0;
}
