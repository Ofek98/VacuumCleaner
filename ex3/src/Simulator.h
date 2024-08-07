#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "common.h"
#include "House.h"
#include "common/BatteryMeter.h"
#include "common/DirtSensor.h"
#include "common/WallSensor.h"
#include "common/AbstractAlgorithm.h"
#include <stdexcept>
#include <fstream>
#include <string>
#include <filesystem>
#include <memory>
#include <regex>
#include <chrono>

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
    size_t initial_dirt;
    std::unique_ptr<AbstractAlgorithm> algo;
    std::string algo_name;
    std::filesystem::path input_file_path;

    float decreaseBattery();
    
    void charge();

public:
    int run(bool write_output_file);

    bool readHouseFile(std::string file_path);

    void setAlgorithm(std::unique_ptr<AbstractAlgorithm> algo);

    void setAlgorithmName(std::string algo_name);

    size_t getMaxSteps();

    size_t getInitialDirt();
};

#endif // SIMULATOR_H
