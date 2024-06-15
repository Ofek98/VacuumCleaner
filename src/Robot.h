#ifndef ROBOT_H
#define ROBOT_H

#include "Algorithm.h"
#include "House.h"
#include "common.h"
#include <vector>

class Robot {
public:
    Robot(House& house, int battery_capacity);
    std::vector<Location> getSurroundingWalls();
    Location getLocation();
    Step performNextStep();
    int getCurrentLocationDirt();

private: 
    Algorithm algo;
    House house
    Location location;
    int battery_capacity;
    float battery_left;
    std::vector<Step> steps_taken;
    std::vector<Location> docking_station_path;
    bool is_charging;
    void clean();
    void charge();
    void move(Location next_loc, bool is_returning);
};

#endif // ROBOT_H