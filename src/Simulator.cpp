#include "Simulator.h"

Simulator::Simulator(size_t max_battery_steps, House::Matrix tiles): house(tiles), robot(house, max_battery_steps) {}

RunResults Simulator::run(size_t total_steps) {
    for (size_t i = 0; i < total_steps; i++)
    {
        robot.performNextStep();
    }

    RunResults res;
    res.steps_taken = robot.getStepsTaken();
    res.battery_left = robot.getBatteryLeft();
    res.dirt_left = house.getTotalDirt();
    res.is_docking = robot.getCurrentCoords() == house.getDockingStationCoords();
    return res;
}