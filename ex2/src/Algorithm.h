#ifndef ALGORITHM_H
#define ALGORITHM_H

/**
 * @file Algorithm.h
 * @brief This file contains the declaration of the Algorithm class.
 */

#include "common.h"
#include "abstract_algorithm.h"


/**
 * @class Algorithm
 * @brief The Algorithm class represents the algorithm used by the robot to make decisions.
 */
class Algorithm : public AbstractAlgorithm {
    const BatteryMeter* battery_meter;
    const DirtSensor* dirt_sensor;
    const WallsSensor* wall_sensor;
    std::size_t max_steps;
    bool is_charging; /**< A flag indicating if the robot is currently charging. */
    Coords last_direction; /**< The last direction the robot moved in. */
    int battery_capacity; /**< The battery capacity of the robot. */
    std::vector<Coords> path_from_docking_station; /**< The path from the docking station to the current location. */

public:
	void setMaxSteps(std::size_t maxSteps) override;
	void setWallsSensor(const WallsSensor& wallSensor) override;
	void setDirtSensor(const DirtSensor& dirtSensor) override;
	void setBatteryMeter(const BatteryMeter& batteryMeter) override;
    Step nextStep() override;
};



#endif // ALGORITHM_H
