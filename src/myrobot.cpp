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
 * @brief Reads the input file and extracts the input values.
 * 
 * @param file The input file stream.
 * @return InputValues The extracted input values.
 */
InputValues readInputFile(std::ifstream& file)
{
    InputValues input_values;

    std::string line;
    int line_count = 0;
    size_t max_line_length = 0;
    while(std::getline(file, line))
    {
        // TODO: not number error handling / less then one line / max_battery_steps not specified
        if(line_count == 0) {
            if(!is_number(line)) {
                std::cerr << "Error: max_battery_steps is not a non negative integer" << std::endl;
                input_values.success = false;
                return input_values;
            }
            input_values.max_battery_steps = std::stoi(line);
        }  
        else if(line_count == 1) {
            if(!is_number(line)) {
                std::cerr << "Error: total_steps is not a non negative integer" << std::endl;
                input_values.success = false;
                return input_values;
            }
            input_values.total_steps = std::stoi(line);
        }
        else if(max_line_length < line.size()) 
            max_line_length = line.size();
        line_count++;
    }
    if(line_count<3){
        std::cerr << "Error: input file should have at least 3 lines" << std::endl;
        input_values.success = false;
        return input_values;
    }

    input_values.tiles = House::Matrix(max_line_length, line_count-2); 

    int y = 0;
    file.clear();                 // Clear EOF flag
    file.seekg(0, std::ios::beg); // Move to the beginning of the file
    
    // skip the first 2 lines
    std::getline(file, line);
    std::getline(file, line);
    bool is_there_docking_station;

    try{
    // fill wall-surrounded matrix with tiles by input file
        while(std::getline(file, line))
        {
            for (size_t x = 0; x < max_line_length; x++)
            {
                // +1 takes the added surrounding walls into account
                input_values.tiles(x+1, y+1) = x < line.length() ? House::Tile(line[x]) : House::Tile(0);
                int tile_status = input_values.tiles(x+1, y+1).getStatus();
                if(tile_status == DOCKING_STATION) {
                    if (is_there_docking_station) {
                        std::cerr << "Error: There can be only one docking station" << std::endl;
                        input_values.success = false;
                        return input_values;
                }
                    input_values.docking_station = Coords(x+1,y+1);
                    is_there_docking_station = true;
                }
                else if(tile_status > 0) {
                    input_values.total_dirt += tile_status;
                }
            }
            y++;
            }
    }
    catch(const std::runtime_error& e) {
        std::cerr << "Error in input file reading: " << e.what() << std::endl;
        input_values.success = false;
        return input_values;
    }
    if(!is_there_docking_station) {
        std::cerr << "Error: No docking station found in the input file" << std::endl;
        input_values.success = false;
        return input_values;
    }
    input_values.success = true;
    return input_values;
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
int main(int argc, char* argv[]) {
    // Check the number of arguments
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <house_input_file>" << std::endl;
        return EXIT_FAILURE;
    }

    //  open input file
    std::ifstream file(argv[1]);
    if(!file)
    {
        std::cerr << "Input file \"" << argv[1] << "\" does not exist" << std::endl;
        return EXIT_FAILURE;
    }

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

