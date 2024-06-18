#ifndef ALGORITHM_H
#define ALGORITHM_H

#include "Robot.h"
#include "common.h"

class Algorithm {
public:
    Algorithm(Robot& robot);

    Step decide_next_step();

private:
    Robot& robot;
    bool is_charging;
    Cords last_direction;  
    int battery_capacity;
    int distance_from_docking_station;
    Cords offset_from_docking_station;
    Step moveToFreeDirection();
};

#endif // ALGORITHM_H