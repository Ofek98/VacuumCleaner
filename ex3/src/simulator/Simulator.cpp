/**
 * @file Simulator.cpp
 * @brief This file contains the implementation of the Simulator class.
 */

#include "Simulator.h"
#include <sstream>


std::string Simulator::run() {
    
    rres.steps_taken.reserve(maxSteps+1);
    auto timeout = std::chrono::milliseconds(maxSteps);
    auto start = std::chrono::high_resolution_clock::now();

    rres.log_info += "Docking Station Location: " + (std::ostringstream() << house.getDockingStationCoords()).str() + "\n";

    for (size_t i = 0; i < maxSteps+1 && !rres.finished; i++)
    {
        // Append details to the log string before executing each step
        rres.log_info += "******* Step " + std::to_string(i + 1) + " *******\n";
        rres.log_info += "Current Location: " + (std::ostringstream() << location).str() + "\n";  // Using the overloaded << operator
        rres.log_info += "Remaining Steps Number: " + std::to_string(maxSteps - i) + "\n";
        rres.log_info += "Battery Left: " + std::to_string(battery_left) + "\n";
        rres.log_info += "House Total Dirt: " + std::to_string(house.getTotalDirt()) + "\n";
        

        if(battery_left == 0 && location != house.getDockingStationCoords()) {
            break;
        }

        Step next_step;
        try {
            next_step = algo->nextStep();
        }
        catch (const std::exception& e) {
            std::string what = e.what();
            return "Caught an exception from algorithm: " + what;
        }
        catch (...) {
            return "Unknown exception from algorithm";
        }
        if((std::chrono::high_resolution_clock::now() - start) > timeout) {
            rres.timeout_reached = true;
            return "";
        }

        if(next_step == Step::Finish) {
            rres.finished = true;
            rres.steps_taken.push_back('F');
            rres.log_info += "Chosen Step: " + (std::ostringstream() << next_step).str() + "\n";
            break;
        }
        else if(i == maxSteps) {
            break;
        }

        switch (next_step)
        {
        case Step::North:
            location += Direction::North;
            rres.steps_taken.push_back('N');
            decreaseBattery();
            break;
        
        case Step::South:
            location += Direction::South;
            rres.steps_taken.push_back('S');
            decreaseBattery();
            break;

        case Step::East:
            location += Direction::East;
            rres.steps_taken.push_back('E');
            decreaseBattery();
            break;
        
        case Step::West:
            location += Direction::West;
            rres.steps_taken.push_back('W');
            decreaseBattery();
            break;

        case Step::Stay:
            if(location == house.getDockingStationCoords())
                charge();
            else {
                house.cleanOnce(location);
                decreaseBattery();
            }
            rres.steps_taken.push_back('s');
            break;

        default:
            break;
        }

        if(house.isWall(location)) {
            break;
        }

        rres.log_info += "Chosen Step: " + (std::ostringstream() << next_step).str() + "\n";
        rres.log_info += "\n"; // line break
    }

    return "";
}

size_t Simulator::calcScoreAndWriteResults(bool write_output_file) {
    size_t dirt_left = house.getTotalDirt();
    size_t num_steps = rres.steps_taken.size();
    Coords docking_station = house.getDockingStationCoords();
    bool in_dock = location == docking_station;
    size_t score;
    // calculating score
    if(rres.timeout_reached) {
        score = maxSteps * 2 + initial_dirt * 300 + 2000;
    }
    else if(!rres.finished && battery_left <= 0 && !in_dock) { // DEAD
        score = maxSteps + dirt_left * 300 + 2000;
    }
    else if(rres.finished && !in_dock) {
        score = maxSteps + dirt_left * 300 + 3000;
    }
    else {
        score = num_steps + dirt_left * 300 + (in_dock ? 0 : 1000);
    }

    std::ofstream output_file(house_file_path.filename().replace_extension("").string() + "-" + algo_name + ".txt"); // Open output file

    if (!output_file) {
        std::cerr << "Failed to open the output file" << std::endl;
        return 0;
    }

    if(write_output_file) {
        output_file << "NumSteps = " << num_steps - rres.finished << std::endl;
        output_file << "DirtLeft = " << dirt_left << std::endl;
        output_file << "Status = " << (rres.finished ? "FINISHED" : (battery_left > 0 ? "WORKING" : "DEAD")) << std::endl;
        output_file << "InDock = " << (in_dock ? "TRUE" : "FALSE") << std::endl;
        output_file << "Score = " << score << std::endl;
        output_file << "Steps:" << std::endl;
        for (size_t i = 0; i < rres.steps_taken.size(); i++)
        {
            output_file << rres.steps_taken[i];
        }
        output_file << std::endl;

        output_file.close();
    }

    std::ofstream log_file(house_file_path.filename().replace_extension("").string() + "-" + algo_name + ".log"); // Open log file

    if (!log_file) {
        std::cerr << "Failed to open the output file" << std::endl;
        return 0;
    }
    log_file << rres.log_info;
    log_file.close();

    return score;
}


Simulator::HouseValues Simulator::readHouseFile(std::filesystem::path house_file_path)
{
    HouseValues hv;

    //  open input file
    std::ifstream file(house_file_path);
    if(!file)
    {
        hv.error_message = "Input file \"" +  house_file_path.string() + "\" does not exist";
        file.close();
        return hv;
    }

    hv.house_path = house_file_path;

    std::string line;
    size_t rows_num, cols_num;
    std::regex max_steps_pattern(R"(^MaxSteps\s*=\s*(\d+)$)");
    std::regex max_battery_pattern(R"(^MaxBattery\s*=\s*(\d+)$)");
    std::regex rows_num_pattern(R"(^Rows\s*=\s*(\d+)$)");
    std::regex cols_num_pattern(R"(^Cols\s*=\s*(\d+)$)");
    std::regex line_patterns[4] = {max_steps_pattern, max_battery_pattern, rows_num_pattern, cols_num_pattern};
    std::size_t* vals[4] = {&hv.maxSteps, &hv.battery_capacity, &rows_num, &cols_num};


    // reading simulation parameters
    for (size_t line_number = 1; line_number <= 5; line_number++)
    {
        if(!std::getline(file, line)) {
            hv.error_message = "Error: input file should have at least 5 lines";
            file.close();
            return hv;
        }

        // Title - ignore and continue
        if(line_number == 1)
            continue;

        std::string line_error = "";
        std::smatch matches;
        if(std::regex_match(line, matches, line_patterns[line_number-2])) {
            try {
                *(vals[line_number-2]) = std::stoi(matches[1]);
            } catch (std::invalid_argument& e) {
                line_error = "invalid number format";               
            } catch (std::out_of_range& e) {
                line_error = "number out of range";             
            }
        }
        else {
            line_error = "invalid run parameter";
        }

        if(line_error != "") {
            hv.error_message = "Error: " + line_error + "; in this line(" + std::to_string(line_number) + "): \"" + line + "\"";
            file.close();
            return hv;
        }
    }
    hv.tiles = House::Matrix(rows_num, cols_num);
    hv.total_dirt = 0;

    bool docking_station_found = false;

    // fill matrix with tiles by input file
    try{
        for (size_t i = 0; i < rows_num && std::getline(file, line); i++)
        {
            for (size_t j = 0; j < line.length() && j < cols_num; j++)
            {
                hv.tiles(i, j) = House::Tile(line[j]);
                int tile_status = hv.tiles(i, j).getStatus();
                if(tile_status == DOCKING_STATION) {
                    if (docking_station_found) {
                        hv.error_message = "Error: There can be only one docking station";
                        file.close();
                        return hv;
                    }
                    hv.docking_station = Coords(i,j);
                    docking_station_found = true;
                }
                else if(tile_status > 0) {
                    hv.total_dirt += tile_status;
                }
            }
        }
    }
    catch(const std::runtime_error& e) {
        std::string what = e.what();
        hv.error_message = "Error in input file reading: " + what;
        file.close();
        return hv;
    }
    if(!docking_station_found) {
        hv.error_message = "Error: No docking station found in the input file";
        file.close();
        return hv;
    }
    file.close();

    return hv;
}

void Simulator::setHouseValues(Simulator::HouseValues hv) { // copy constructor is called on purpose
    house.setTiles(std::move(hv.tiles));
    house.setDockingStation(hv.docking_station);
    house.setTotalDirt(hv.total_dirt);
    house_file_path = hv.house_path;
    maxSteps = hv.maxSteps;
    battery_capacity = hv.battery_capacity;
    battery_left = battery_capacity;
    location = house.getDockingStationCoords();
    initial_dirt = house.getTotalDirt();
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

std::filesystem::path Simulator::getHousePath() {
    return house_file_path;
}

std::string Simulator::getAlgorithmName() {
    return algo_name;
}