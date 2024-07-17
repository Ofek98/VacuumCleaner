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
    size_t battery_left; /**< The remaining battery level of the robot after the simulation. */
    bool is_docking; /**< Indicates whether the robot is at the docking station at the end of the simulation. */
};

/**
 * @brief The Simulator class represents a vacuum cleaner simulator.
 */
class Simulator {
    
    class HouseSensor {
    protected:
        House* house;
        Simulator& parent;
    public: 
        HouseSensor(Simulator& parent);
        virtual void setHouse(House* house);
    };

    class HouseWallsSensor : public WallsSensor, public HouseSensor {
        using HouseSensor::HouseSensor;
    public:
	    bool isWall(Direction d) const override;
    };

    class HouseBatteryMeter : public BatteryMeter, public HouseSensor {
        using HouseSensor::HouseSensor;
    public:
	    std::size_t getBatteryState() const override;
    };

    class HouseDirtSensor : public DirtSensor, public HouseSensor {
        using HouseSensor::HouseSensor;
    public:
	    int dirtLevel() const override;
    };

    House house; /**< The house object representing the house to be cleaned in the simulation. */
    HouseWallsSensor wallsSensor = HouseWallsSensor(*this);
    HouseBatteryMeter batteryMeter = HouseBatteryMeter(*this);
    HouseDirtSensor dirtSensor = HouseDirtSensor(*this);
    std::size_t maxSteps;
    Coords location; /**< The current location of the robot. */
    std::size_t battery_capacity; /**< The battery capacity of the robot. */
    float battery_left; /**< The remaining battery level of the robot. */
    Algorithm algo;

public:
    /**
     * @brief Runs the simulation.
     * @return The results of the simulation run.
     */
    RunResults run();

    void readHouseFile(std::ifstream& file);

    void setAlgorithm(Algorithm algo);
};

#endif // SIMULATOR_H
