#ifndef ALGORITHM_H
#define ALGORITHM_H


#include "common.h"

class Robot;

class Algorithm {
public:
    Algorithm(Robot* robot);

    Step decide_next_step();

private:
    Robot* robot;
    bool is_charging;
    Coords last_direction;  
    int battery_capacity;
    std::vector<Coords> path_from_docking_station;
    Step moveToFreeDirection();
};

#include "Robot.h"

#endif // ALGORITHM_H
