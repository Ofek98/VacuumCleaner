/**
 * @file Simulator.cpp
 * @brief This file contains the implementation of the Simulator class.
 */

#include "Simulator.h"

RunResults Simulator::run() {
    for (size_t i = 0; i < total_steps; i++)
    {
        algo.nextStep();
    }

    RunResults res;
    res.steps_taken = robot.getStepsTaken();
    res.battery_left = robot.getBatteryLeft();
    res.dirt_left = house.getTotalDirt();
    res.is_docking = res.steps_taken.back().type == CHARGE;
    return res;
}


/**
 * @brief Reads the input file and extracts the input values.
 * 
 * @param file The input file stream.
 * @return InputValues The extracted input values.
 */
void readHouseFile(std::ifstream& file)
{
    InputValues input_values;

    std::string line;
    int line_count = 0;
    size_t max_line_length = 0;
    while(std::getline(file, line))
    {
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


void Simulator::setAlgorithm(Algorithm algo) {
	algo.setMaxSteps(maxSteps);
	algo.setWallsSensor(wallsSensor);
	algo.setDirtSensor(dirtSensor);
	algo.setBatteryMeter(batteryMeter);
}

Simulator::HouseSensor::HouseSensor(Simulator& parent) : parent(parent) {}

void Simulator::HouseSensor::setHouse(House* house) {
    this->house = house;
}



bool Simulator::HouseWallsSensor::isWall(Direction d) const {
    return house->isWall(parent.location + d);
    
}