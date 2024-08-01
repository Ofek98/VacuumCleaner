#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "common.h"
#include "House.h"
#include "Algorithm.h"
#include <stdexcept>
#include <fstream>
#include <string>
#include <filesystem>

/**
 * @brief The Simulator class represents a vacuum cleaner simulator.
 */
class Simulator {
    
    class SimulatorSensor {
    protected:
        Simulator& parent;
    public: 
        SimulatorSensor(Simulator& parent);
    };

    class HouseWallsSensor : public WallsSensor, public SimulatorSensor {
        using SimulatorSensor::SimulatorSensor;
    public:
	    bool isWall(Direction d) const override;
    };

    class HouseBatteryMeter : public BatteryMeter, public SimulatorSensor {
        using SimulatorSensor::SimulatorSensor;
    public:
	    std::size_t getBatteryState() const override;
    };

    class HouseDirtSensor : public DirtSensor, public SimulatorSensor {
        using SimulatorSensor::SimulatorSensor;
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
    std::filesystem::path input_file_path;

    float decreaseBattery();
    
    void charge();

public:
    bool run();

    bool readHouseFile(std::string file_path);

    void setAlgorithm(Algorithm&& algo);
};

#endif // SIMULATOR_H
