#include <iostream>
#include <string>
#include <fstream>

#define ERROR -1
#define SUCCESS 0

int openFile(char* file_name)
{
    std::ifstream file(file_name);

    if(!file)
    {
        return ERROR;
    }

    return SUCCESS;
}


int main(int argc, char* argv[]) {
    // Check the number of arguments
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <house_input_file>" << std::endl;
        return 1;
    }

    if(openFile(argv[1]))
    {
        std::cerr << "Input file \"" << argv[1] << "\" does not exist" << std::endl;
    }



    return 0;
}

