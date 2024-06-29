#ifndef SIMULATOR_H
#define SIMULATOR_H

/**
 * @file Simulator.h
 * @brief This file contains the declaration of the Simulator class and the RunResults struct.
 */


#include "common.h"
#include "House.h"
#include "Robot.h"
#include <stdexcept>
#include <fstream>
#include <string>

/**
 * @brief The RunResults struct represents the results of a simulation run.
 */
struct RunResults
{
    std::vector<Step> steps_taken; /**< The steps taken by the robot during the simulation. */
    size_t dirt_left; /**< The amount of dirt left in the house after the simulation. */
    float battery_left; /**< The remaining battery level of the robot after the simulation. */
    bool is_docking; /**< Indicates whether the robot is at the docking station at the end of the simulation. */
};

/**
 * @brief The Simulator class represents a vacuum cleaner simulator.
 */
class Simulator {
    House house; /**< The house object representing the house to be cleaned in the simulation. */
    Robot robot; /**< The robot object representing the vacuum cleaner. */
public:
    /**
     * @brief Constructs a Simulator object with the specified parameters.
     * @param max_battery_steps The maximum number of steps the robot's battery can last.
     * @param tiles The matrix representing the layout of the house.
     * @param docking_station The coordinates of the docking station.
     * @param total_dirt The total amount of dirt in the house.
     */
    Simulator(size_t max_battery_steps, House::Matrix tiles, Coords docking_station, size_t total_dirt);

    /**
     * @brief Runs the simulation for the specified number of steps.
     * @param total_steps The total number of steps to run the simulation for.
     * @return The results of the simulation run.
     */
    RunResults run(size_t total_steps);
};

#endif // SIMULATOR_H
