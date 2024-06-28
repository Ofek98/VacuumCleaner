#ifndef ROBOT_H
#define ROBOT_H

/**
 * @file Robot.h
 * @brief This file contains the declaration of the Robot class.
 */

#include "Algorithm.h"
#include "House.h"
#include "common.h"

/**
 * @class Robot
 * @brief Represents a vacuum cleaning robot.
 */
class Robot {
public:
    /**
     * @brief Constructs a Robot object.
     * @param house The house to clean.
     * @param battery_capacity The battery capacity of the robot.
     */
    Robot(House& house, size_t battery_capacity);

    /**
     * @brief Retrieves the surrounding walls of the robot's current location.
     * @return A boolean array representing the presence of walls in the surrounding directions.
     */
    bool* getSurroundingWalls();

    /**
     * @brief Performs the next step of the cleaning algorithm.
     * @return The step performed by the robot.
     */
    Step performNextStep();

    /**
     * @brief Retrieves the amount of dirt at the robot's current location.
     * @return The amount of dirt at the current location.
     */
    size_t getCurrentCoordsDirt() const;

    /**
     * @brief Retrieves the remaining battery level of the robot.
     * @return The remaining battery level as a float value.
     */
    float getBatteryLeft() const;

    /**
     * @brief Retrieves the steps taken by the robot during the cleaning process.
     * @return A vector of Step objects representing the steps taken.
     */
    std::vector<Step> getStepsTaken();

private: 
    House& house; /**< The house to clean. */
    Coords location; /**< The current location of the robot. */
    size_t battery_capacity; /**< The battery capacity of the robot. */
    float battery_left; /**< The remaining battery level of the robot. */
    std::vector<Step> steps_taken; /**< The steps taken by the robot. */
    Algorithm algo; /**< The cleaning algorithm used by the robot. */

    /**
     * @brief Cleans the current location once.
     */
    void clean();

    /**
     * @brief Charges the robot's battery once.
     */
    void charge();

    /**
     * @brief Moves the robot to the specified location.
     * @param next_loc The coordinates of the next location to move to.
     */
    void move(Coords next_loc);

    /**
     * @brief Decreases the battery level of the robot by one step.
     * @return The updated battery level as a float value.
     */
    float decreaseBattery(); 
};

#endif // ROBOT_H