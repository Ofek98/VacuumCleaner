/**
 * @file Simulator.cpp
 * @brief This file contains the implementation of the Simulator class.
 */

#include "Simulator.h"


int Simulator::run(bool write_output_file) {
    std::vector<char> steps_taken;
    steps_taken.reserve(maxSteps+1);
    bool finished = false;
    auto timeout = std::chrono::milliseconds(maxSteps);
    auto start = std::chrono::high_resolution_clock::now();

    for (size_t i = 0; i < maxSteps+1 && !finished; i++)
    {
        if(battery_left == 0 && location != house.getDockingStationCoords()) {
            break;
        }

        Step next_step = algo->nextStep();

        if((std::chrono::high_resolution_clock::now() - start) > timeout) {
            return -1;
        }

        if(next_step == Step::Finish) {
            finished = true;
            steps_taken.push_back('F');
            break;
        }
        else if(i == maxSteps) {
            break;
        }

        switch (next_step)
        {
        case Step::North:
            location += Direction::North;
            steps_taken.push_back('N');
            decreaseBattery();
            break;
        
        case Step::South:
            location += Direction::South;
            steps_taken.push_back('S');
            decreaseBattery();
            break;

        case Step::East:
            location += Direction::East;
            steps_taken.push_back('E');
            decreaseBattery();
            break;
        
        case Step::West:
            location += Direction::West;
            steps_taken.push_back('W');
            decreaseBattery();
            break;

        case Step::Stay:
            if(location == house.getDockingStationCoords())
                charge();
            else {
                house.cleanOnce(location);
                decreaseBattery();
            }
            steps_taken.push_back('s');
            break;

        default:
            break;
        }

        if(house.isWall(location)) {
            break;
        }
    }

    size_t dirt_left = house.getTotalDirt();
    size_t num_steps = steps_taken.size();
    Coords docking_station = house.getDockingStationCoords();
    bool in_dock = location == docking_station;
    size_t score;
    // calculating score
    if(!finished && battery_left <= 0 && !in_dock) { // DEAD
        score = maxSteps + dirt_left * 300 + 2000;
    }
    else if(finished && !in_dock) {
        score = maxSteps + dirt_left * 300 + 3000;
    }
    else {
        score = num_steps + dirt_left * 300 + (in_dock ? 0 : 1000);
    }

    // writing to output file
    if(write_output_file) {
        std::string output_file_name = input_file_path.filename().replace_extension("").string() + "-" + algo_name + ".txt";
        std::ofstream file(input_file_path.parent_path() /= std::filesystem::path(output_file_name)); // Open output file

        if (!file) {
            std::cerr << "Failed to open the output file" << std::endl;
            return 0;
        }

        file << "NumSteps = " << num_steps - finished << std::endl;
        file << "DirtLeft = " << dirt_left << std::endl;
        file << "Status = " << (finished ? "FINISHED" : (battery_left > 0 ? "WORKING" : "DEAD")) << std::endl;
        file << "InDock = " << (in_dock ? "TRUE" : "FALSE") << std::endl;
        file << "Score = " << score << std::endl;
        file << "Steps:" << std::endl;
        for (size_t i = 0; i < steps_taken.size(); i++)
        {
            file << steps_taken[i];
        }
        file << std::endl;

        file.close();
    }

    return score;
}


bool Simulator::readHouseFile(std::string house_file_path)
{
    //  open input file
    std::ifstream file(house_file_path);
    if(!file)
    {
        std::cerr << "Input file \"" <<  house_file_path << "\" does not exist" << std::endl;
        file.close();
        return false;
    }

    input_file_path = house_file_path;

    std::string line;
    size_t rows_num, cols_num;
    std::regex max_steps_pattern(R"(^MaxSteps\s*=\s*(\d+)$)");
    std::regex max_battery_pattern(R"(^MaxBattery\s*=\s*(\d+)$)");
    std::regex rows_num_pattern(R"(^Rows\s*=\s*(\d+)$)");
    std::regex cols_num_pattern(R"(^Cols\s*=\s*(\d+)$)");
    std::regex line_patterns[4] = {max_steps_pattern, max_battery_pattern, rows_num_pattern, cols_num_pattern};
    std::size_t* vals[4] = {&maxSteps, &battery_capacity, &rows_num, &cols_num};


    // reading simulation parameters
    for (size_t line_number = 1; line_number <= 5; line_number++)
    {
        if(!std::getline(file, line)) {
            std::cerr << "Error: input file should have at least 5 lines" << std::endl;
            file.close();
            return false;
        }

        // Title - ignore and continue
        if(line_number == 1)
            continue;

        std::smatch matches;
        if(std::regex_match(line, matches, line_patterns[line_number-2])) {
            try {
                *(vals[line_number-2]) = std::stoi(matches[1]);
            } catch (std::invalid_argument& e) {
                std::cerr << "Error: Invalid number format" << std::endl;
            } catch (std::out_of_range& e) {
                std::cerr << "Error: Number out of range" << std::endl;
            }
        }
        else {
            std::cerr << "Error: invalid run parameter" << std::endl;
            return false;
        }
    }

    Coords docking_station;
    House::Matrix tiles = House::Matrix(rows_num, cols_num);
    std::size_t total_dirt = 0;

    bool is_there_docking_station = false;

    // fill matrix with tiles by input file
    try{
        for (size_t i = 0; i < rows_num && std::getline(file, line); i++)
        {
            for (size_t j = 0; j < line.length() && j < cols_num; j++)
            {
                tiles(i, j) = House::Tile(line[j]);
                int tile_status = tiles(i, j).getStatus();
                if(tile_status == DOCKING_STATION) {
                    if (is_there_docking_station) {
                        std::cerr << "Error: There can be only one docking station" << std::endl;
                        file.close();
                        return false;
                    }
                    docking_station = Coords(i,j);
                    is_there_docking_station = true;
                }
                else if(tile_status > 0) {
                    total_dirt += tile_status;
                }
            }
        }
    }
    catch(const std::runtime_error& e) {
        std::cerr << "Error in input file reading: " << e.what() << std::endl;
        file.close();
        return false;
    }
    if(!is_there_docking_station) {
        std::cerr << "Error: No docking station found in the input file" << std::endl;
        file.close();
        return false;
    }

    house.setTiles(std::move(tiles));
    house.setDockingStation(docking_station);
    house.setTotalDirt(total_dirt);

    battery_left = battery_capacity;
    location = house.getDockingStationCoords();
    initial_dirt = house.getTotalDirt();

    file.close();
    return true;
}

void Simulator::setAlgorithm(std::unique_ptr<AbstractAlgorithm> algo) {
    this->algo = std::move(algo);
	this->algo->setMaxSteps(maxSteps);
	this->algo->setWallsSensor(wallsSensor);
	this->algo->setDirtSensor(dirtSensor);
	this->algo->setBatteryMeter(batteryMeter);
}

void Simulator::setAlgorithmName(std::string algo_name) {
    this->algo_name = algo_name;
}

Simulator::SimulatorSensor::SimulatorSensor(Simulator& parent) : parent(parent) {}

bool Simulator::HouseWallsSensor::isWall(Direction d) const {
    return parent.house.isWall(parent.location + d);
}

std::size_t Simulator::HouseBatteryMeter::getBatteryState() const {
    return parent.battery_left;
}

int Simulator::HouseDirtSensor::dirtLevel() const {
    return parent.house.getDirtLevel(parent.location);
}

void Simulator::charge() {
    battery_left = std::min((float)battery_capacity, battery_left + ((float)battery_capacity)/20);
}

float Simulator::decreaseBattery() {
    if(battery_left == 0) {
        // We shouldn't reach here
        throw std::runtime_error("Battery is dead and cannot perform the requested move!");
    }
    return battery_left--;
}

size_t Simulator::getMaxSteps() {
    return maxSteps;
}

size_t Simulator::getInitialDirt() {
    return initial_dirt;
}