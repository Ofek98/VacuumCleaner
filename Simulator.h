#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "common.h"
#include "House.h"
#include "Robot.h"
#include <stdexcept>
#include <fstream>
#include <vector>
#include <string>


class Simulator {
    Robot robot;
    House house;
public:
    Simulator(size_t max_battery_steps, size_t total_steps, House::Matrix tiles);
    run(size_t total_steps);

};

#endif // SIMULATOR_H
