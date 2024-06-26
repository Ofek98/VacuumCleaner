#ifndef ROBOT_H
#define ROBOT_H

#include "Algorithm.h"
#include "House.h"
#include "common.h"
#include <vector>

class Robot {
public:
    Robot(House& house, size_t battery_capacity);
    int* getSurroundingWalls() const;
    Step performNextStep();
    size_t getCurrentCordsDirt() const;
    float getBatteryLeft() const;
    void setAlgorithm();

private: 
    House house;
    Coords location;
    Coords docking_station;
    size_t battery_capacity;
    float battery_left;
    std::vector<Step> steps_taken;
    std::vector<Coords> docking_station_path;
    Algorithm algo;

    void clean();
    void charge();
    void move(Coords next_loc, bool is_returning);
    float decreaseBattery(); 
};

#endif // ROBOT_H