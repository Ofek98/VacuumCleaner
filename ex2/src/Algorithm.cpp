/**
 * @file Algorithm.cpp
 * @brief Implementation file for the Algorithm class.
 */
#include "Algorithm.h"
#include <cstdlib>
#include <iostream>

Step Algorithm::nextStep() {
    return Step::Finish;
}

void Algorithm::setMaxSteps(std::size_t maxSteps) {
    max_steps = maxSteps;
}
void Algorithm::setWallsSensor(const WallsSensor& wallSensor) {
    wall_sensor = &wallSensor;
}
void Algorithm::setDirtSensor(const DirtSensor& dirtSensor) {
    dirt_sensor = &dirtSensor;
}
void Algorithm::setBatteryMeter(const BatteryMeter& batteryMeter) {
    battery_meter = &batteryMeter;
}