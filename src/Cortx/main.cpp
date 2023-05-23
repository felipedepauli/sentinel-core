/**
 * @file   main.cpp
 * @brief  The Cortx's Main File.
 * @details This is the main entry point of the Cortex program. It processes the command line arguments, 
 *          sets up signal handling, and runs the Cortex class. The Cortex class is responsible for the main logic of the program.
 *
 * Created by:
 *   - Felipe Camargo de Pauli
 *   - fcdpauli@gmail.com
 *   - 2023-04-15
 *
 * This file is part of the Cortex project:
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

#include <iostream>     // std::cerr
#include "Cortx.hpp"    // Cortx
#include <csignal>      // Linux signals handler

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

    // 01. Verify if the two arguments were passed: IP and Port.
    if (argc != 3) {
        std::cerr << "Usage: client <ip> <port>\n";
        return 1;
    }

    // 02. Get the IP and Port.
    std::string ip = argv[1];
    unsigned short port = std::atoi(argv[2]);

    // 03. Get the responsability to handle the system signals.
    signal(SIGINT, handleSignal);

    // 04. Instantiate the class Cortx.
    Cortx cortex(ip, port);

    // 05. Run the class Cortx and keep the program running.
    cortex.run();

    return 0;
}
