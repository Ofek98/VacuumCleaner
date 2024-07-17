#ifndef ALGORITHM_H
#define ALGORITHM_H

/**
 * @file Algorithm.h
 * @brief This file contains the declaration of the Algorithm class.
 */

#include "common.h"
#include "abstract_algorithm.h"

class Robot;

/**
 * @class Algorithm
 * @brief The Algorithm class represents the algorithm used by the robot to make decisions.
 */
class Algorithm : public AbstractAlgorithm {
    BatteryMeter* battery_meter;
    DirtSensor* dirt_sensor;
    WallsSensor* wall_sensor;
    std::size_t max_steps;
    bool is_charging; /**< A flag indicating if the robot is currently charging. */
    Coords last_direction; /**< The last direction the robot moved in. */
    int battery_capacity; /**< The battery capacity of the robot. */
    std::vector<Coords> path_from_docking_station; /**< The path from the docking station to the current location. */

    /**
     * @brief Randomly moves the robot to a free (non-wall) direction.
     * @return The step to take to move to a free direction.
     */
    Step moveToFreeDirection();

public:
	void setMaxSteps(std::size_t maxSteps) override;
	void setWallsSensor(const WallsSensor& wallSensor) override;
	void setDirtSensor(const DirtSensor& dirtSensor) override;
	void setBatteryMeter(const BatteryMeter& batteryMeter) override;
    Step Algorithm::nextStep() override;
};


#include "Robot.h"

#endif // ALGORITHM_H
