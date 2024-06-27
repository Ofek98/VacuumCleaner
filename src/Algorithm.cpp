#include "Algorithm.h"
#include <cstdlib>
#include <iostream>

Algorithm::Algorithm(Robot* robot) : is_charging(false), last_direction({-1,-1}) {}

Step Algorithm::moveToFreeDirection(){
    bool* surrounding_walls = robot->getSurroundingWalls(); 
    std::vector<int> freeDirections;
    // Collect indices of directions without walls
    for (int i = 0; i < 4; ++i) {
        if (surrounding_walls[i]) {  
            freeDirections.push_back(i);
        }
    }
    if (freeDirections.empty()) {
        // No free directions - can happen only if we stuck on docking station
        return {CHARGE, DIFFLOCATION}; 
    }

    int randomIndex = rand() % freeDirections.size();
    Coords next_direction = DIRECTIONS[freeDirections[randomIndex]];
    last_direction = next_direction;
    return {MOVE, next_direction};
}

Step Algorithm::decide_next_step(){
    
    if (last_direction == NO_DIRECTION) {
        battery_capacity = robot->getBatteryLeft();
        if (battery_capacity < 2){
            return {CHARGE, DIFFLOCATION};
        }
        Step next_step = moveToFreeDirection();
        path_from_docking_station.push_back(next_step.coords);
    }
    else {
        if (is_charging && robot->getBatteryLeft() < battery_capacity){
            return {CHARGE, DIFFLOCATION};
        }
        if (!is_charging && path_from_docking_station.size() == 0 && robot->getBatteryLeft() < battery_capacity){
            is_charging == true;
            return {CHARGE, DIFFLOCATION};
        }
        if (robot->getBatteryLeft()-2 < path_from_docking_station.size()) {
            
            Step res = {MOVE, path_from_docking_station.back().reverse()};
            path_from_docking_station.pop_back();
            return res;
        }
        if (robot->getCurrentCoordsDirt() > 0){
            return {CLEAN, DIFFLOCATION};
        }
        else{
            is_charging = false;
            bool* surrounding_walls = robot->getSurroundingWalls(); 
            for (int i = 0; i < 4; ++i) {
                if (DIRECTIONS[i] == last_direction) {
                    if (!surrounding_walls[i]){
                        path_from_docking_station.push_back(last_direction);
                        return {MOVE,last_direction};
                    }
                    else{
                        Step next_step = moveToFreeDirection();
                        path_from_docking_station.push_back(next_step.coords);
                        return next_step;
                    }
                }
            }

        }
    }
}