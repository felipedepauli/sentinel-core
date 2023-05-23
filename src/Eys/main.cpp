/**
 * @file   main.cpp
 * @brief  The Server's Main File.
 *
 * Created by:
 *   - Felipe Camargo de Pauli
 *   - fcdpauli@gmail.com
 *   - 2023-04-15
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

#include <iostream>
#include <boost/asio.hpp>
#include "Eys.hpp"

/**
 * @brief Main entry point for the Eys server. 
 * Checks if the port is provided. If yes, it instantiates the service and runs it.
 * 
 * @param argc The number of command-line arguments.
 * @param argv The command-line arguments.
 * @return int Returns 0 on successful program termination, 1 if the server port is not provided.
 */
int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "[cli::err]Usage: ./eyes_server <port>\n";
        return 1;
    }

    try {       
        Eys myEys(std::atoi(argv[1]));
        myEys.run();
        std::cout << "[cli::inf] Server started on port " << argv[1] << std::endl;
        
    } catch (std::exception &e) {
        std::cerr << "[cli::inf] Exception: " << e.what() << "\n";
    }

    return 0;
}
