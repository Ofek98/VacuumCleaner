#include "Algorithm.h"
#include <cstdlib>
#include <iostream>

Algorithm::Algorithm(Robot& robot)
    : robot(robot), is_charging(false), last_direction({-1,-1}),distance_from_docking_station(0){
}

Step Algorithm::moveToFreeDirection(){
    int* surrounding_walls = robot.getSurroundingWalls(); 
    std::vector<int> freeDirections;
    // Collect indices of directions without walls
    for (int i = 0; i < 4; ++i) {
        if (surrounding_walls[i] == 0) {  
            freeDirections.push_back(i);
        }
    }
    if (freeDirections.empty()) {
    // No free directions
    return {CHARGE, DIFFLOCATION}; 
    }

    int randomIndex = rand() % freeDirections.size();
    Coords next_direction = DIRECTIONS[freeDirections[randomIndex]];
    last_direction = next_direction;
    return {MOVE, next_direction};
}

Step Algorithm::decide_next_step(){
    
    if (last_direction == {-1,-1}) {
        battery_capacity = robot.getBatteryLeft();
        if (battery_capacity < 2){
            return {CHARGE, DIFFLOCATION};
        }
        return moveToFreeDirection();
    }
    else {
        if (is_charging && robot.getBatteryLeft() < battery_capacity){
            return {CHARGE, DIFFLOCATION};
        }
        if (!is_charging && distance_from_docking_station == 0 && robot.getBatteryLeft < battery_capacity){
            is_charging == true;
            return {CHARGE, DIFFLOCATION};
        }
        if (robot.getBatteryLeft()-2 < distance_from_docking_station) {
            distance_from_docking_station--;
            return {RETURN, DIFFLOCATION};
        }
        if (robot.getDirtLevel() > 0){
            return {CLEAN, DIFFLOCATION};
        }
        else{
            is_charging = false;
            int* surrounding_walls = robot.getSurroundingWalls(); 
            for (int i = 0; i < 4; ++i) {
                if (surrounding_walls[i] == last_direction) {
                    if (surrounding_walls[i] == 0){
                        return {MOVE,last_direction};
                    }
                    else{
                         return moveToFreeDirection();
                    }
                }
            }

        }
    }
}