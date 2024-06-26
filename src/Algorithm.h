#ifndef ALGORITHM_H
#define ALGORITHM_H

/**
 * @file Algorithm.h
 * @brief This file contains the declaration of the Algorithm class.
 */

#include "common.h"

class Robot;

/**
 * @class Algorithm
 * @brief The Algorithm class represents the algorithm used by the robot to make decisions.
 */
class Algorithm {
public:
    /**
     * @brief Constructs an Algorithm object.
     * @param robot A pointer to the Robot object.
     */
    Algorithm(Robot* robot);

    /**
     * @brief Decides the next step for the robot.
     * @return The next step to take.
     */
    Step decide_next_step();

private:
    Robot* robot; /**< A pointer to the Robot object. */
    bool is_charging; /**< A flag indicating if the robot is currently charging. */
    Coords last_direction; /**< The last direction the robot moved in. */
    int battery_capacity; /**< The battery capacity of the robot. */
    std::vector<Coords> path_from_docking_station; /**< The path from the docking station to the current location. */

    /**
     * @brief Randomly moves the robot to a free (non-wall) direction.
     * @return The step to take to move to a free direction.
     */
    Step moveToFreeDirection();
};


#include "Robot.h"

#endif // ALGORITHM_H
