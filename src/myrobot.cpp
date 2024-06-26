#include <iostream>
#include <string>
#include <fstream>
#include "House.h"
#include "Robot.h"

#define ERROR -1
#define SUCCESS 0

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
   
    input_values.success = true;
    return input_values;
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
    }
    catch(const std::runtime_error& e) {
        std::cerr << "Error in input file reading: " << e.what() << std::endl;
    }
    catch(...) {
        std::cerr << "Unknown error in input file reading" << std::endl;
    }
    
    if(!input_values.success) {
        return ERROR;
    }

    Simulator simulator = Simulator(input_values.)


    return 0;
}

