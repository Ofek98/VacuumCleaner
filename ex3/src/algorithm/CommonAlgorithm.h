#ifndef COMMON_ALGORITHM_H
#define COMMON_ALGORITHM_H

/**
 * @file commonAlgorithm.h
 * @brief This file contains the declaration of the commonAlgorithm class.
 */

#include "../common/enums.h"
#include "../common/BatteryMeter.h"
#include "../common/WallSensor.h"
#include "../common/DirtSensor.h"
#include "../common/AbstractAlgorithm.h"
#include "AlgorithmRegistration.h"
#include "../common.h"
#include <unordered_map>
#include <deque>
#include <cstdlib>
#include <iostream>
#include <cmath>

/**
 * @class CommonAlgorithm
 * @brief The CommonAlgorithm class represents the common functionality used by the robot to make decisions.
 */
class CommonAlgorithm : public AbstractAlgorithm {

    public:
        
        const BatteryMeter* battery_meter;
        const DirtSensor* dirt_sensor;
        const WallsSensor* wall_sensor;
        std::size_t max_steps;

        void setMaxSteps(std::size_t maxSteps) override;
        void setWallsSensor(const WallsSensor& wallSensor) override;
        void setDirtSensor(const DirtSensor& dirtSensor) override;
        void setBatteryMeter(const BatteryMeter& batteryMeter) override;
        virtual Step nextStep() override = 0;
    
    protected:
        CommonAlgorithm(bool is_deterministic);
          // Function to be called by derived classes
        void updateDetailsAboutCurrLocAndItsNeighbors();
        void updateDistFromDocking(int dist);
        bool appendNeighbors(const Coords& current, std::deque<Coords>& queue, std::unordered_map<Coords, Coords>& parents, bool to_docking);
        CoordsVector bfs(bool to_docking, size_t limiting_factor, bool is_deterministic);
        CoordsVector constructNextPath(size_t limiting_factor, bool is_deterministic);
        Step marchTheNextStepOfThePath();
        size_t stepsNumberToCharge(size_t amount);
        Step nextStep(bool is_deterministic);
        CoordsVector createPathByParents(Coords start,Coords target,std::unordered_map<Coords,Coords> parents);
        
        bool is_deterministic;
        CoordsVector path;
        std::unordered_map<Coords, float> coords_info;
        Coords curr_loc;
        size_t max_battery;
        size_t remaining_steps;
        size_t dist_from_docking;
        bool is_dist_from_docking_updated;
        size_t charging_cap;
        bool is_charging_cap_updated;
};

#endif // COMMON_ALGORITHM_H
