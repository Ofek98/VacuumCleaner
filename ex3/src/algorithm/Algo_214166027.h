#ifndef ALGO_214166027_H
#define ALGO_214166027_H

/**
 * @file Algo_214166027.h
 * @brief This file contains the declaration of the Algo_214166027 class.
 */

#include "../common.h"
#include "../common/AbstractAlgorithm.h"
#include <unordered_map>
#include <deque>
#include <cstdlib>
#include <iostream>
#include <cmath>
#include "AlgorithmRegistration.h"

/**
 * @class Algo_214166027
 * @brief The Algo_214166027 class represents the Algo_214166027 used by the robot to make decisions.
 */
class Algo_214166027 : public AbstractAlgorithm {

    public:
        const BatteryMeter* battery_meter;
        const DirtSensor* dirt_sensor;
        const WallsSensor* wall_sensor;
        std::size_t max_steps;

        Algo_214166027();
        void setMaxSteps(std::size_t maxSteps) override;
        void setWallsSensor(const WallsSensor& wallSensor) override;
        void setDirtSensor(const DirtSensor& dirtSensor) override;
        void setBatteryMeter(const BatteryMeter& batteryMeter) override;
        Step nextStep() override;

    private:
        void updateDetailsAboutCurrLocAndItsNeighbors();
        void updateDistFromDocking(int dist);
        bool appendNeighbors(const Coords& current, std::deque<Coords>& queue,std::unordered_map<Coords,Coords> &parents, bool to_docking);
        CoordsVector bfs(bool to_docking, size_t limiting_factor);
        CoordsVector constructNextPath(size_t limiting_factor);
        Step marchTheNextStepOfThePath();
        size_t stepsNumberToCharge(size_t amount);
        CoordsVector path;
        std::unordered_map<Coords,float> coords_info;
        Coords curr_loc;
        size_t max_battery;
        size_t remaining_steps;
        size_t dist_from_docking;
        bool is_dist_from_docking_updated;
        size_t charging_cap;
        bool is_charging_cap_updated;
};



#endif // ALGORITHM_H
