/**
 * @file Algorithm.cpp
 * @brief Implementation file for the Algorithm class.
 */
#include "Algorithm.h"
#include <cstdlib>
#include <iostream>

Algorithm::Algorithm(Robot* robot) : robot(robot), is_charging(false), last_direction({-1,-1}) {}

Step Algorithm::moveToFreeDirection(){
    bool* surrounding_walls = robot->getSurroundingWalls(); 
    std::vector<int> freeDirections;
    // Collect indices of directions without walls
    for (int i = 0; i < 4; ++i) {
        if (!surrounding_walls[i]) {  
            freeDirections.push_back(i);
        }
    }
    delete surrounding_walls;

    if (freeDirections.empty()) {
        // No free directions - can happen only if we stuck on docking station
        return {CHARGE, DIFFLOCATION}; 
    }

    int randomIndex = rand() % freeDirections.size();
    Coords next_direction = DIRECTIONS[freeDirections[randomIndex]];
    last_direction = next_direction;
    return {MOVE, next_direction};
}

Step Algorithm::nextStep() {
    // relevant for first step only
    if (last_direction == NO_DIRECTION) {
        battery_capacity = battery_meter.getBatteryState();
        if (battery_capacity < 2){
            return {CHARGE, DIFFLOCATION};
        }
        Step next_step = moveToFreeDirection();
        path_from_docking_station.push_back(next_step.coords);
        return next_step;
    }

    // regular cases:
    
    if (is_charging && battery_meter.getBatteryState() < battery_capacity){
        return {CHARGE, DIFFLOCATION};
    }
    if (!is_charging && path_from_docking_station.size() == 0 && battery_meter.getBatteryState() < battery_capacity){
        is_charging = true;
        moveToFreeDirection(); //We won't use this step so it will only just pick new direction to move on after the charging
        return {CHARGE, DIFFLOCATION};
    }
    if (battery_meter.getBatteryState()-2 < path_from_docking_station.size()) {
        
        Step res = {MOVE, path_from_docking_station.back().reverse()};
        path_from_docking_station.pop_back();
        return res;
    }
    if (dirt_sensor.dirtLevel() > 0){
        return {CLEAN, DIFFLOCATION};
    }

    is_charging = false;
    bool* surrounding_walls = robot->getSurroundingWalls(); 
    for (int i = 0; i < 4; ++i) {
        if (DIRECTIONS[i] == last_direction) {
            if (!surrounding_walls[i]){
                path_from_docking_station.push_back(last_direction);
                delete surrounding_walls;
                return {MOVE,last_direction};
            }
            else{
                Step next_step = moveToFreeDirection();
                path_from_docking_station.push_back(next_step.coords);
                delete surrounding_walls;
                return next_step;
            }
        }
    }     
    // We would never reach here
    return {CHARGE,DIFFLOCATION};   
}

void Algorithm::setMaxSteps(std::size_t maxSteps) {
    max_steps = maxSteps;
}
void Algorithm::setWallsSensor(const WallsSensor& wallSensor) {
    wall_sensor = wallSensor;
}
void Algorithm::setDirtSensor(const DirtSensor& dirtSensor) {
    dirt_sensor = dirtSensor;
}
void Algorithm::setBatteryMeter(const BatteryMeter& batteryMeter) {
    battery_meter = batteryMeter;
}