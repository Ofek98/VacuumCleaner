#include <iostream>
#include <string>
#include <fstream>
#include "House.h"
#include "Simulator.h"

struct InputValues
{   
    bool success = false;
    size_t max_battery_steps;
    size_t total_steps;
    House::Matrix tiles;
};

InputValues readInputFile(std::ifstream& file)
{
    InputValues input_values;

    std::string line;
    int line_count = 0;
    size_t max_line_length = 0;
    while(std::getline(file, line))
    {
        if(line_count == 0) {
            input_values.max_battery_steps = std::stoi(line);
        }  
        else if(line_count == 1) {
            input_values.total_steps = std::stoi(line);
        }
        if(max_line_length < line.length())
            max_line_length = line.length();
        line_count++;
    }

    
    input_values.tiles = House::Matrix(max_line_length, line_count-2, true); 

    int x = 0, y = 0;
    file.clear();                 // Clear EOF flag
    file.seekg(0, std::ios::beg); // Move to the beginning of the file

    // skip the first 2 lines
    std::getline(file, line);
    std::getline(file, line);

    while(std::getline(file, line))
    {
        for (size_t x = 0; x < input_values.tiles.getDimX(); x++)
        {
            // +1 takes the added surrounding walls into account
            input_values.tiles(x+1, y+1) = x < line.length() ? House::Tile(line[x]) : House::Tile(0);
        }
        y++;
    }
   
    for (size_t x = 0; x < tiles.getDimX(); x++)
    {
        for (size_t y = 0; y < tiles.getDimY(); y++)
        {
            if(tiles(x, y).getStatus() == DOCKING_STATION) {
                
            }
        }
    }

    input_values.success = true;
    return input_values;
}


bool writeOutputFile(RunResults res)
{
    std::ofstream file("output.txt"); // Open output file

    if (!file) {
        std::cerr << "Failed to open the file" << std::endl;
        return false;
    }

    file << "Steps performed by the vaccum cleaner:" << std::endl;

    for(Step step: res.steps_taken) {
        file << "(" << step.coords.x << ", " << step.coords.y << "): " << step.type << std::endl;
    }

    file << "Total number of steps performed: " << res.steps_taken.size() << std::endl;

    file << (res.battery_left ? "Vaccum cleaner still has" + std::to_string(res.battery_left) + "battery" : "Vaccum cleaner is dead") << std::endl;

    file << "Mission" << ((!res.dirt_left && res.is_docking) ? "succeeded" : "failed") << std::endl;

    file << "Amount of dirt left in the house: " << res.dirt_left << std::endl;
    file << "Vaccum cleaner is " << (res.is_docking ? "" : "not ") << "at the docking station" << std::endl;

    file.close();

    return 0;
}

int main(int argc, char* argv[]) {
    // Check the number of arguments
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <house_input_file>" << std::endl;
        return 1;
    }

    //  open input file
    std::ifstream file(argv[1]);
    if(!file)
    {
        std::cerr << "Input file \"" << argv[1] << "\" does not exist" << std::endl;
    }

    InputValues input_values;
    try {
        input_values = readInputFile(file);

        if(!input_values.success) {
            return EXIT_FAILURE;
        }

        Simulator simulator = Simulator(input_values.max_battery_steps, input_values.tiles);

        writeOutputFile(simulator.run(input_values.total_steps));

        return EXIT_SUCCESS;
    }
    catch(const std::runtime_error& e) {
        std::cerr << "Error in input file reading: " << e.what() << std::endl;
    }
    catch(...) {
        std::cerr << "Unknown error" << std::endl;
    }

    return EXIT_FAILURE;
}

