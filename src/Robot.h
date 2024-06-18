#ifndef ROBOT_H
#define ROBOT_H

#include "Algorithm.h"
#include "House.h"
#include "common.h"
#include <vector>

class Robot {
public:
    Robot(House& house, int battery_capacity);
    int* getFreeCells();
    Step performNextStep();
    int getCurrentCordsDirt();
    float getBatteryLeft();

private: 
    House house;
    Cords location;
    Cords docking_station;
    int battery_capacity;
    float battery_left;
    std::vector<Step> steps_taken;
    std::vector<Cords> docking_station_path;
    Algorithm algo;

    void clean();
    void charge();
    void move(Cords next_loc, bool is_returning);
§    float decreaseBattery(); 
};

#endif // ROBOT_H