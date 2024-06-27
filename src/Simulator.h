#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "common.h"
#include "House.h"
#include "Robot.h"
#include <stdexcept>
#include <fstream>
#include <string>

struct RunResults
{
    std::vector<Step> steps_taken;
    size_t dirt_left;
    size_t battery_left;
    bool is_docking;
};

class Simulator {
    House house;
    Robot robot;
public:
    Simulator(size_t max_battery_steps, House::Matrix tiles, Coords docking_station);
    RunResults run(size_t total_steps);
};

#endif // SIMULATOR_H
