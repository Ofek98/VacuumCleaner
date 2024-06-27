#ifndef ROBOT_H
#define ROBOT_H

#include "Algorithm.h"
#include "House.h"
#include "common.h"
#include <vector>

class Robot {
public:
    Robot(House& house, size_t battery_capacity);
    bool* getSurroundingWalls();
    Step performNextStep();
    size_t getCurrentCoordsDirt() const;
    float getBatteryLeft() const;
    void setAlgorithm();

private: 
    House house;
    Coords location;
    size_t battery_capacity;
    float battery_left;
    std::vector<Step> steps_taken;
    Algorithm algo;

    void clean();
    void charge();
    void move(Coords next_loc);
    float decreaseBattery(); 
};

#endif // ROBOT_H