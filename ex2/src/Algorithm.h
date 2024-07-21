#ifndef ALGORITHM_H
#define ALGORITHM_H

/**
 * @file Algorithm.h
 * @brief This file contains the declaration of the Algorithm class.
 */

#include "common.h"
#include "abstract_algorithm.h"
#include <unordered_map>
#include <deque>


/**
 * @class Algorithm
 * @brief The Algorithm class represents the algorithm used by the robot to make decisions.
 */
class Algorithm : public AbstractAlgorithm {

    public:
        const BatteryMeter* battery_meter;
        const DirtSensor* dirt_sensor;
        const WallsSensor* wall_sensor;
        std::size_t max_steps;

        Algorithm();
        void setMaxSteps(std::size_t maxSteps) override;
        void setWallsSensor(const WallsSensor& wallSensor) override;
        void setDirtSensor(const DirtSensor& dirtSensor) override;
        void setBatteryMeter(const BatteryMeter& batteryMeter) override;
        Step nextStep() override;

    private:
        void updateDetailsAboutCurrLocAndItsNeighbors();
        bool appendNeighbors(const Coords& current, std::deque<Coords>& queue,std::unordered_map<Coords,Coords> parents);
        std::vector<Coords> bfs();
        Step marchTheNextStepOfThePath();
        std::vector<Coords> path;
        std::unordered_map<Coords,float> coords_info;
        Coords curr_loc;
        size_t max_battery;
        size_t dist_from_docking;
        bool is_returning;
        size_t remaining_steps;
        bool first_iteration;
        size_t limiting_factor;
};



#endif // ALGORITHM_H
