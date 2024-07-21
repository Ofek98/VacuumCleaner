#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "common.h"
#include "House.h"
#include "Algorithm.h"
#include <stdexcept>
#include <fstream>
#include <string>

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
    std::string input_file_name;

    float decreaseBattery();
    void charge();

public:
    bool run();

    bool readHouseFile(std::string file_path);

    void setAlgorithm(Algorithm&& algo);
};

#endif // SIMULATOR_H
