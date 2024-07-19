/**
 * @file myrobot.cpp
 * @brief This file contains the implementation of a vacuum cleaner simulation program.
 */

#include <iostream>
#include <string>
#include <fstream>
#include "House.h"
#include "Simulator.h"


/**
 * @brief The main function of the program.
 * 
 * @param argc The number of command-line arguments.
 * @param argv The array of command-line arguments.
 * @return int The exit status of the program.
 */
int main(int argc, char** argv) {
	Simulator simulator;
    std::string houseFilePath;

    // Check the number of arguments
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <house_input_file>" << std::endl;
        return EXIT_FAILURE;
    }

    houseFilePath = argv[1];
    if(!simulator.readHouseFile(houseFilePath)) {
        std::cerr << "Error in input file: ^^^" << std::endl;
        return EXIT_FAILURE;
    }
    
	Algorithm algo;
    simulator.setAlgorithm(algo);
	simulator.run();
}

