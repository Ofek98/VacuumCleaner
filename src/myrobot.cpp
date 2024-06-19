#include <iostream>
#include <string>
#include <fstream>
#include "House.h"
#include "Robot.h"

#define ERROR -1
#define SUCCESS 0


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


    try
    {
        House house = House(file);
        Robot robot = Robot(house, 3);
        
    }
    catch(const std::runtime_error& e)
    {
        std::cerr << "Error in house creation: " << e.what() << std::endl;
    }
    

    return 0;
}

