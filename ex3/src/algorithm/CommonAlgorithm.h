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
#include <algorithm>
#include <random>

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
    
    protected:
        CommonAlgorithm(bool is_deterministic);
          // Function to be called by derived classes
        void updateInformation(size_t limiting_factor);
        void updateDistFromDocking(int dist);
        void appendNeighbors(const Coords& current, std::deque<Coords>& queue,std::unordered_map<Coords,Coords> &parents, bool to_docking, std::deque<Coords> &candidates, int i, size_t limiting_factor);
        CoordsVector bfs(size_t limiting_factor, bool updating_distances_from_docking);
        CoordsVector constructNextPath(size_t limiting_factor);
        Step marchTheNextStepOfThePath();
        size_t stepsNumberToCharge(size_t amount);
        Step nextStep();
        CoordsVector createPathByParents(Coords start,Coords target,std::unordered_map<Coords,Coords> parents);

        
        bool is_deterministic;
        CoordsVector path;
        std::unordered_map<Coords, float> coords_info;
        std::unordered_map<Coords, size_t> distances_from_docking;
        std::unordered_map<Coords,Coords> path_from_docking_parents;
        Coords curr_loc;
        size_t max_battery;
        size_t remaining_steps;
        size_t charging_cap;
        bool is_charging_cap_updated;
};

#endif // COMMON_ALGORITHM_H
