/**
 * @file Algorithm.cpp
 * @brief Implementation file for the Algorithm class.
 */
#include "Algorithm.h"
#include <cstdlib>
#include <iostream>


Algorithm::Algorithm(){
    curr_loc = Coords(0, 0);
    coords_info[Coords(0,0)] = UNEXPLORED;
    dist_from_docking = 0;
    is_dist_from_docking_updated = true;
}
/*
Updating curr_loc maping in coords_info to its real level,
and adding its neighbors if needed (we can only know if they're walls or explorable) 
*/
void Algorithm::updateDetailsAboutCurrLocAndItsNeighbors(){
        coords_info[curr_loc] = curr_loc == Coords(0, 0)? DOCKING_STATION : dirt_sensor->dirtLevel();
        for(int i = 0; i < 4; i++){
            Direction dir = static_cast<Direction>(i);
            Coords loc = curr_loc + dir;
            if (coords_info.find(loc) == coords_info.end()){ 
                coords_info[loc] = wall_sensor->isWall(dir)? WALL : UNEXPLORED;
            }
        }
}

void Algorithm::updateDistFromDocking(int dist){
    dist_from_docking = dist;
    is_dist_from_docking_updated = true;
}

/*
Adds the relevant neighbors of current to the queue and adds their mapping in parents map. 
A neighbor is relevant if we know it can be accessed, and we didn't travel it yet during the bfs run.
It returns if we can finish our bfs run, which means:
If its_returning - we can generate a path to the docking station. 
If not - we can generate a path to an unexplored or a cleanable cell.
*/
bool Algorithm::appendNeighbors(const Coords& current, std::deque<Coords>& queue,std::unordered_map<Coords,Coords> &parents, bool to_docking){
    bool can_finish = false;
    for(int i = 0; i < 4; i++){
        Direction dir = static_cast<Direction>(i);
        Coords neighbor = current + dir;
        
        if (neighbor == Coords(0,0)){
            std::cout << "In appendNeighbors: Docking was founded"<< std::endl;
            if (!(parents.find(neighbor) == parents.end())){
                std::cout <<"First condition wasn't met" << std::endl;
            }
            if (!(coords_info.find(neighbor) != coords_info.end())){
                std::cout <<"First condition wasn't met" << std::endl;
            }
            if (!(coords_info[neighbor] != WALL)){
                std::cout <<"First condition wasn't met"<< std::endl;
            }
        }
        
        if (parents.find(neighbor) == parents.end() && coords_info.find(neighbor) != coords_info.end() && coords_info[neighbor] != WALL){
            //We haven't visited neighbor, it is a known cell which is not a wall
            if (neighbor == Coords(0,0)){
                //std::cout << "Docking was found in appendNeighbors, and we entered the critical part"<< std::endl;
            }
            parents[neighbor] = current;
            queue.push_back(neighbor);
            //if (neighbor == Coords(0,0)){
                //std::cout << "to_docking? " <<to_docking<< std::endl;
            //}
            if (to_docking){
                if (neighbor == Coords(0,0)){ //We found the docking so we can return immediately
                    //std::cout << "WE ENTERED HERE"<< std::endl;
                    can_finish = true;
                    //std::cout << "AppendNeighbors can_finish :" << can_finish << std::endl;
                    return can_finish;
                }
            }
            else{
                if (coords_info[neighbor] > 0){ 
                    // We found dirty or unexplored cell, so we can finish the bfs after finishing to construct this level
                    can_finish = true;
                }
            }
        }
    }
    return can_finish;
}

/*
Creates a path from start to target using the parents mapping;
*/
CoordsVector createPathByParents(Coords start,Coords target,std::unordered_map<Coords,Coords> parents){
    //std::cout << "In createPathByParents"<< std::endl;
    //std::cout << "Start: "<< start << ", Target: "<< target << std::endl;
    CoordsVector next_path;
    next_path.push_back(target);
    Coords current = target;
    while (parents[current] != start){
        current = parents[current];
        next_path.push_back(current);
    }
    
    return next_path; //There is a copy elision compiler optimization so we didn't use std::move here 
}

/*
Generates a path to the closest reachable (within the steps limitations) cleanable sell, 
or to the docking station if there isn't such a cell
*/
CoordsVector Algorithm::bfs(bool to_docking, size_t limiting_factor){
    std::cout << "In BFS"<< std::endl;
    Coords target = Coords(0,0);
    bool can_finish = false;
    int max_iterations = to_docking? limiting_factor : ((limiting_factor-1)/ 2);
    std::cout<<"Max iterations: "<< max_iterations <<std::endl;
    /*
    If we just need to return to the docking, we could use all the steps.
    If we want to clean something, we will need at least one step for the cleaning,
    and another 2 X the length of the path to our current location steps 
    */
    std::deque<Coords> queue = {curr_loc}; 
    std::unordered_map<Coords,Coords> parents;//We will use it to map children to their parents and to remember where we already visited
    parents[curr_loc] = curr_loc; //Arbitrary mapping, we won't use it as we just need to have curr_loc in the parents; 
    for (int i = 0; i < max_iterations; i++){
        std::cout << "Iteration num :" << i << std::endl;
        size_t len = queue.size();
        if (len == 0){
            //No unknown or dirty cells are reachable
            return {};
        }
        for (size_t j = 0; j < len; j++){ //Popping out all the coords from the current level and entering their relevant neighbors
            Coords current = queue.front();
            queue.pop_front();
            if(appendNeighbors(current, queue, parents, to_docking)){//Adds neighbors to queue and parents if they're not there yet
                can_finish = true;
            } 
            //std::cout << "First can_finish :" << can_finish << std::endl;
            //and checks if we can finish the run (by to_docking)
        }
        //std::cout << "Second can_finish :" << can_finish << std::endl;
        if (can_finish){
            std::cout << "Can finish == true" << std::endl;
            if (!to_docking) { /*finds the dirtiest known cell from the reachable in the shortest distance,
            in case of not returning to the docking station*/
                Coords candidate = queue.front();
                float candidate_status = coords_info[candidate];
                queue.pop_front();
                while (!queue.empty()){
                    if (coords_info[queue.front()] > candidate_status) {
                        candidate = queue.front();
                        candidate_status = coords_info[candidate];
                    }
                    queue.pop_front();
                }
                target = candidate;
            }
            return createPathByParents(curr_loc,target,parents);;
        }
    }
    //No dirty or unknown cell was reachable within max_iterations allowed steps
    return {};
}

CoordsVector Algorithm::constructNextPath(size_t limiting_factor) {
    CoordsVector path_to_docking = bfs(true,limiting_factor);
    std::cout << "Constructed path to docking: " << path_to_docking << std::endl;
    std::cout << "Limiting factor that will be used for the path to the cleanable cell: " << limiting_factor-path_to_docking.size() << std::endl;
    CoordsVector path_to_closet_cleanable_cell = bfs(false,limiting_factor-path_to_docking.size()); 
    //limiting_factor-path_to_docking.size() as limiting_factor so the robot will be able to return to its original place with enough steps to return to the docking after it
    return path_to_closet_cleanable_cell.empty()? path_to_docking : path_to_closet_cleanable_cell;
}

/*
Based on the path updates curr_loc and the next step that will be taken, 
as well as dist_from_docking that's getting updated by whether the robot is returning or not
*/
Step Algorithm::marchTheNextStepOfThePath(){
    bool in_the_way_to_docking = (path.front() == Coords(0,0));
    Coords next_loc = path.back();
    path.pop_back();
    Step res = Step(next_loc-curr_loc);
    curr_loc = next_loc;
    if (in_the_way_to_docking){
        updateDistFromDocking(path.size());//This is a shortest path to the docking station so we know we can update it
    }
    else{
        is_dist_from_docking_updated = false; //We can't guarantee real distance from the docking in this case 
    }
    return res;
}

Step Algorithm::nextStep() {
    std::cout << "***************** Step " << max_steps-remaining_steps << " *****************"<<std::endl;
    std::cout <<"Path is: "<<path<<std::endl;
    std::cout << "Battery State: "<< battery_meter->getBatteryState() << std::endl;
    Step res;
    //Limiting_factor is the actual number of steps until robot must return to the docking_station
    //We consider limiting_factor-1 for the finishing step 
    size_t limiting_factor = std::min(remaining_steps-1, battery_meter->getBatteryState());
    //std::cout << "Limiting factor: " << limiting_factor << std::endl;

    /*
    First we make sure that the current location and its neighbors are updated in coords_info.
    For curr_loc we can know its dirt level, and for the neighbors we can know if they're walls
    */
    if (coords_info[curr_loc] == UNEXPLORED) {
        updateDetailsAboutCurrLocAndItsNeighbors();
    }

    //Now we're deciding the next step

    /*
    Condition 1: curr_loc is cleanable
    */
    if (dirt_sensor->dirtLevel() >= 1){
        std::cout <<"Condition 1 - cleanable"<<std::endl;
        if (!is_dist_from_docking_updated) { //Make sure dist_from_docking_is_updated. If we're on the way to the docking, it will be updated
            std::cout << "Dist from docking is getting updated"<<std::endl;
            CoordsVector path_to_docking = bfs(true,limiting_factor);
            std::cout << "path_to_docking: "<<path_to_docking<<std::endl;
            updateDistFromDocking(path_to_docking.size()); //The length of the path to the docking received from bfs 
        }
        std::cout << "Curr loc: " << curr_loc << ",dist_from_docking: " << dist_from_docking<<std::endl;
        if (dist_from_docking + 1 <= limiting_factor){ //Enough steps to clean and return to the docking station
            std::cout <<"Condition 1, enough steps"<<std::endl;
            coords_info[curr_loc] -= 1; //The robot cleans the cell
            remaining_steps -=1; 
            return Step::Stay;
        }
    }

    /*
    Condition 2: Curr_loc is not cleanable, and there is already a path that the algo constructed,
    so the robot will march another step in this path towards its destination.
    Note this is not else condition regarding to condition 1, because it is possible
     to have a non empty path to the docking station which some of its nodes are cleanable
    */
    if (!path.empty()){
        res = marchTheNextStepOfThePath();
    }

    /*
    Condition 3: There is no existing path that the algo constructed 
    */
    else{
        /*
        Condition 3.1: At the docking station
        */
        if (curr_loc == Coords(0,0)){
            if (is_charging_cap_updated){
                if(battery_meter->getBatteryState() >= charging_cap){
                    is_charging_cap_updated = false;
                    path = bfs(false,limiting_factor);
                    if (!path.empty()){
                    res = marchTheNextStepOfThePath();
                    }
                    else{
                        res = Step::Finish;
                    }
                }
                else{
                    res = Step::Stay;
                }
            }
            //TODO:document all the new conditions
            else{
                CoordsVector optional_path = bfs(false, std::min(max_battery,remaining_steps-1)); //Checks the path that will be available after fully charging
                if(optional_path.empty()){
                    res = Step::Finish;
                }
                else if (remaining_steps-1 >= 2*optional_path.size()+1+20){//remaining_steps is sufficient to fully charge and clean path's target
                    charging_cap = max_battery;
                    res = Step::Stay;
                } 
                else{ //remaining_steps is insufficient to fully charge and clean path's target
                    if (remaining_steps -1 >= 2 *(2*optional_path.size()+1)){ //We can partly charge and still be able to clean path's target
                        charging_cap = (remaining_steps - 1)/2;
                        res = Step::Stay;
                    }
                    else{ //Can't even partly charge and clean the path's target
                        res = Step::Finish;
                    }
                }
                is_charging_cap_updated = true;
            }
        }
        /*
        Condition 3.2: We have an empty path and we're not at the docking station.
        We will construct our next path to a cleanable/unexplored cell or to the docking station.
        */
        else{ 
            path = constructNextPath(limiting_factor);
            std::cout << "Path after construction: " << path << std::endl;
            res = marchTheNextStepOfThePath();
        }
    }
    remaining_steps -=1;
    return res;
}

void Algorithm::setMaxSteps(std::size_t maxSteps) {
    this->max_steps = maxSteps;
    remaining_steps = maxSteps;
}
void Algorithm::setWallsSensor(const WallsSensor& wallSensor) {
    this->wall_sensor = &wallSensor;
}
void Algorithm::setDirtSensor(const DirtSensor& dirtSensor) {
    this->dirt_sensor = &dirtSensor;
}
void Algorithm::setBatteryMeter(const BatteryMeter& batteryMeter) {
    this->battery_meter = &batteryMeter;
    max_battery = battery_meter->getBatteryState();
    is_charging_cap_updated = true;
    charging_cap = max_battery;
}
//TODO: change to const whatever possible