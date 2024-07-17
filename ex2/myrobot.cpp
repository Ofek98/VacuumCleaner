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
 * @brief Structure to store input values read from the input file.
 */
struct InputValues
{   
    bool success = false; /**< Flag indicating if the input file was read successfully. */
    size_t max_battery_steps; /**< Maximum number of battery steps. */
    size_t total_steps; /**< Total number of steps to be performed. */
    size_t total_dirt = 0; /**< Total amount of dirt in the house. */
    House::Matrix tiles; /**< Matrix representing the house layout. */
    Coords docking_station; /**< Coordinates of the docking station. */
};

/**
 * @brief Determines if a string is a positive integer
 * 
 * @param s the string to be checked if it is a number
 * @return true iff the string is a number
*/
bool is_number(const std::string& s) //Taken from https://stackoverflow.com/questions/4654636/how-to-determine-if-a-string-is-a-number-with-c
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}



/**
 * @brief Writes the output file with the simulation results.
 * 
 * @param res The simulation results.
 * @param input_file_name The input file name, for making the output file name.
 * @return bool True if the output file was written successfully, false otherwise.
 */
bool writeOutputFile(RunResults res, std::string input_file_name)
{
    std::ofstream file("output_" + input_file_name); // Open output file

    if (!file) {
        std::cerr << "Failed to open the file" << std::endl;
        return false;
    }

    file << "Steps performed by the vacuum cleaner:" << std::endl;

    for(Step step: res.steps_taken) {

        file << "(" << step.coords.x << ", " << step.coords.y << "): " << stepTypeToString(step.type) << std::endl;
    }

    file << "Total number of steps performed: " << res.steps_taken.size() << std::endl;

    file << (res.battery_left ? "Vacuum cleaner still has " + std::to_string(res.battery_left) + " battery" : "Vacuum cleaner is dead") << std::endl;

    file << "Mission " << ((!res.dirt_left && res.is_docking) ? "succeeded" : "failed") << std::endl;

    file << "Amount of dirt left in the house: " << res.dirt_left << std::endl;
    file << "Vacuum cleaner is " << (res.is_docking ? "" : "not ") << "at the docking station" << std::endl;

    file.close();

    return 0;
}

/**
 * @brief The main function of the program.
 * 
 * @param argc The number of command-line arguments.
 * @param argv The array of command-line arguments.
 * @return int The exit status of the program.
 */
int main(int argc, char** argv) {
	Simulator simulator;

    // Check the number of arguments
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <house_input_file>" << std::endl;
        return EXIT_FAILURE;
    }
    else {
        
    }
    //  open input file
    std::ifstream file(argv[1]);
    if(!file)
    {
        std::cerr << "Input file \"" << argv[1] << "\" does not exist" << std::endl;
        return EXIT_FAILURE;
    }

    simulator.readHouseFile(file);
    file.close();
	Algorithm algo;
    simulator.setAlgorithm(algo);
	simulator.run();
}




int main(int argc, char* argv[]) {


    InputValues input_values;
    input_values = readInputFile(file);
    file.close();
    if(!input_values.success) {
        return EXIT_FAILURE;
    }

    Simulator simulator = Simulator(input_values.max_battery_steps, input_values.tiles, input_values.docking_station, input_values.total_dirt);

    writeOutputFile(simulator.run(input_values.total_steps), argv[1]);

    return EXIT_SUCCESS;
    try {

    }
    catch(const std::runtime_error& e) {
        std::cerr << "Error in input file reading: " << e.what() << std::endl;
    }
    catch(...) {
        std::cerr << "Unknown error" << std::endl;
    }

    return EXIT_FAILURE;
}

