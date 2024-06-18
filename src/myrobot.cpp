#include <iostream>
#include <string>
#include <fstream>
#include "House.h"
#include "Robot.h"


#define ERROR -1
#define SUCCESS 0

House buildHouseFromFile(std::ifstream& file)
{
    std::string my_text;
    while(getline(file, my_text))
    {
        
    }
}


int main(int argc, char* argv[]) {
    // Check the number of arguments
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <house_input_file>" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1]);
    if(!file)
    {
        std::cerr << "Input file \"" << argv[1] << "\" does not exist" << std::endl;
    }
    buildHouseFromFile(file);
    


    return 0;
}

