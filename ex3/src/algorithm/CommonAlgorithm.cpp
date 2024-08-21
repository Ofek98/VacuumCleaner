/**
 * @file CommonAlgorithm.cpp
 * @brief Implementation file for the CommonAlgorithm class.
 */
#include "CommonAlgorithm.h"


void CommonAlgorithm::setMaxSteps(std::size_t maxSteps){
    this->max_steps = maxSteps + 1;
    remaining_steps = max_steps; // +1 for Finish
}

void CommonAlgorithm::setWallsSensor(const WallsSensor& wallSensor) {
    this->wall_sensor = &wallSensor;
}
void CommonAlgorithm::setDirtSensor(const DirtSensor& dirtSensor) {
    this->dirt_sensor = &dirtSensor;
}
void CommonAlgorithm::setBatteryMeter(const BatteryMeter& batteryMeter) {
    this->battery_meter = &batteryMeter;
    max_battery = battery_meter->getBatteryState();
    is_charging_cap_updated = true;
    charging_cap = max_battery;
}

CommonAlgorithm::CommonAlgorithm(bool is_deterministic) 
    : is_deterministic(is_deterministic) {
    curr_loc = Coords(0, 0);
    coords_info[Coords(0, 0)] = UNEXPLORED;
}
/**
Updating curr_loc maping in coords_info to its real level,
and adding its neighbors if needed (we can only know if they're walls or explorable) 
*/
void CommonAlgorithm::updateInformation(size_t limiting_factor){
    bool added_new_cell = false;
    coords_info[curr_loc] = curr_loc == Coords(0, 0)? DOCKING_STATION : dirt_sensor->dirtLevel();
    for(int i = 0; i < 4; i++){
        Direction dir = static_cast<Direction>(i);
        Coords loc = curr_loc + dir;
        if (coords_info.find(loc) == coords_info.end()){  
            coords_info[loc] = wall_sensor->isWall(dir)? WALL : UNEXPLORED;
            if (coords_info[loc] == UNEXPLORED){
                //An explored cell got revealed
                added_new_cell = true;
            }
        }
    }
    if (added_new_cell){
        bfs(limiting_factor, true); //Distances and paths from docking might need to be updated
        if (!path.empty()){
            path = constructNextPath(limiting_factor); //If we're in the middle of a travel path, there might be a better one now
        }
    }
}

/*
Adds the relevant neighbors of current to the queue and adds their mapping in parents map. 
A neighbor is relevant if we know it can be accessed, and we didn't travel it yet during the bfs run.
It returns if we can finish our bfs run, which means:
If to_docking - we can generate a path to the docking station. 
If not - we can generate a path to an unexplored or a cleanable cell.
*/
void CommonAlgorithm::appendNeighbors(const Coords& current, std::deque<Coords>& queue,std::unordered_map<Coords,Coords> &parents, bool updating_distances_from_docking, std::deque<Coords> &candidates, int i, size_t limiting_factor){
    for(int j = 0; j < 4; j++){
        Direction dir = static_cast<Direction>(j);
        Coords neighbor = current + dir;

        if (parents.find(neighbor) == parents.end() && coords_info.find(neighbor) != coords_info.end() && coords_info[neighbor] != WALL){
            //We haven't visited neighbor, it is a known cell which is not a wall

            parents[neighbor] = current;
            queue.push_back(neighbor);
            int neighbor_distance_from_curr_loc = i+1;
            size_t min_path_length = neighbor_distance_from_curr_loc + distances_from_docking[neighbor] + 1;
            if (!updating_distances_from_docking && min_path_length <= limiting_factor && coords_info[neighbor] > 0){ 
                //We're looking for cleanable cell, the neighbor is indeed cleanable, and we can complete a traversal to it, clean it and return to the docking within the limiting factor
                candidates.push_back(neighbor); 
            }
        }
    }
}


/*
Creates a path from start to target using "parents" map
*/
CoordsVector CommonAlgorithm::createPathByParents(Coords start,Coords target,std::unordered_map<Coords,Coords> parents){
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
If to_docking - creates a shortest path from curr_loc to the docking station.
Else, a shortest path to the closest or unexplored cell (If there are many at the same level - to the known most dirty).
If there is no path that allows us to reach target and go back to the docking safely, returns an empty path.
*/
CoordsVector CommonAlgorithm::bfs(size_t limiting_factor, bool updating_distances_from_docking){
    static std::random_device rd;
    /*
    If we just need to return to the docking, we could use all the steps.
    If we want to explore something, We need half of the remaining steps to return.
    If we're starting at the docking station, we want to be able to clean the explored cell, so we assign one step to this.  
    */
    Coords current = updating_distances_from_docking ? Coords(0,0) : curr_loc;
    std::deque<Coords> queue = {current}; 
    std::unordered_map<Coords, Coords> local_parents;
    std::unordered_map<Coords, Coords>& parents = updating_distances_from_docking ? path_from_docking_parents : local_parents;

    if (updating_distances_from_docking) {
        path_from_docking_parents.clear();  // Reset path_from_docking_parents
    }
    parents[current] = current; //Arbitrary mapping, we won't use it as we just need to have current in the parents mapping; 
    std::deque<Coords> candidates;
    int max_iterations = updating_distances_from_docking? max_steps : limiting_factor; 
    // If we're updating the distances from the docking we want to have information about all the cells that are reachable to the docking at any time, so we perform max_steps iterations
    // Otherwise, we want to search for cleanable cells that we can reach within the limiting_factor steps frame 
    for (int i = 0; i < max_iterations; i++){
        size_t len = queue.size();
        if (len == 0){
            //We've scanned all the cells
            return {};
        }
        for (size_t j = 0; j < len; j++){ //Popping out all the coords from the current level and adding their relevant neighbors
            current = queue.front();
            queue.pop_front();
            if (updating_distances_from_docking){
                distances_from_docking[current] = i; 
            }
            appendNeighbors(current, queue, parents, updating_distances_from_docking, candidates,i, limiting_factor);

        }
        if (!is_deterministic){
            /*
            If not deterministic, we shuffle the order of the Coords in the queue and the candidates for the bfs's target.
            This way, we will pick one of the shortest paths randomly and even the target randomly (from the most prioritized targets)
            */

             // Create a random number generator
            std::mt19937 g(rd());

            // Shuffle the queue and the candidates
            std::shuffle(queue.begin(), queue.end(), g);
            std::shuffle(candidates.begin(), candidates.end(),g);
        }

        if (!candidates.empty()){ // Can happen only when looking for cleanable cells, otherwise we will run until no more reachable cells within the max_steps limit
            Coords candidate = candidates.front();
            float candidate_status = coords_info[candidate];
            candidates.pop_front();
            while (!candidates.empty()){
                if (coords_info[candidates.front()] > candidate_status) {
                    candidate = candidates.front();
                    candidate_status = coords_info[candidate];
                }
                candidates.pop_front();
            }
            return createPathByParents(curr_loc,candidate,parents); 
        }
    }
    //No dirty or unknown cell was reachable within max_iterations allowed steps
    return {};
}

/*
Constructs the robot's next path:
If there's a path to a unexplored/cleanable cell within the limit of limiting_factor and the distance from the docking.
it will construct a path to there. 
Else, it will construct a path to the docking station.
*/
CoordsVector CommonAlgorithm::constructNextPath(size_t limiting_factor) {
    CoordsVector path_to_closet_cleanable_cell = bfs(limiting_factor, false); 
    if (!path_to_closet_cleanable_cell.empty()){
        return path_to_closet_cleanable_cell;
    }
    CoordsVector reversed_path_to_docking = createPathByParents(Coords(0,0),curr_loc,path_from_docking_parents);
    reversed_path_to_docking.push_back(Coords(0,0)); // adding docking station to the end of the path
    std::reverse(reversed_path_to_docking.begin(),reversed_path_to_docking.end());
    reversed_path_to_docking.pop_back(); // removing our current location from the path
    return reversed_path_to_docking;
}

/*
Marches the next step of the path, including needed fields' updates
*/
Step CommonAlgorithm::marchTheNextStepOfThePath(){
    Coords next_loc = path.back();
    path.pop_back();
    Step res = Step(next_loc-curr_loc);
    curr_loc = next_loc;
    return res;
}

/*
Calculates how many steps the robot will need to charge to make battery_state >= amount.
*/
size_t CommonAlgorithm::stepsNumberToCharge(size_t amount){
    size_t amount_left = amount - battery_meter->getBatteryState();
    float charging_size = float(max_battery)/20;
    return std::ceil(float(amount_left)/charging_size);
}

Step CommonAlgorithm::nextStep(){
    Step res;
    /* Limiting_factor is the actual number of steps until robot must return to the docking_station
    We consider limiting_factor-1 for the finishing step */
    size_t limiting_factor = std::min(remaining_steps-1, battery_meter->getBatteryState());
    // 

    /*
    First we make sure that the current location and its neighbors are updated in coords_info.
    For curr_loc we can know its dirt level, and for the neighbors we can know if they're walls
    */
    if (coords_info[curr_loc] == UNEXPLORED) {
        updateInformation(limiting_factor);
    }

    //Now we're deciding the next step

    /*
    Condition 1: curr_loc is cleanable
    */
    if (dirt_sensor->dirtLevel() >= 1 && distances_from_docking[curr_loc] + 1 <= limiting_factor){ //Enough steps to clean and return to the docking station
        coords_info[curr_loc] -= 1; //The robot cleans the cell
        remaining_steps -=1; 
        return Step::Stay;
    }

    /*
    Condition 2: Curr_loc is not cleanable, and there is already a path that the algo constructed,
    so the robot will march another step in this path towards its destination
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
            
            if (is_charging_cap_updated){ //If we know how much we need to charge
                 
                if(battery_meter->getBatteryState() >= charging_cap){ //We charged enough 
                     
                    is_charging_cap_updated = false;
                    path = bfs(limiting_factor, false); //Calculate next path
                    if (!path.empty()){
                        res = marchTheNextStepOfThePath();
                    }
                    else{
                        res = Step::Finish;
                    }
                }
                else{ //We still need to charge
                     
                    res = Step::Stay;
                }
            }
            else{ //Else, we need to calculate how much to charge 
                 
                CoordsVector optional_path = bfs(std::min(max_battery,remaining_steps-1), false); //Checks the shortest path available, assuming battery is not limiting us 
                size_t cleaning_duty_min_steps = 2*optional_path.size()+1; //Steps that it will take to clean the path's target at least once 
                if(optional_path.empty()){ //There is no reachable cleanable cells
                    res = Step::Finish;
                }
                else if (remaining_steps-1 >= cleaning_duty_min_steps+stepsNumberToCharge(max_battery)){//remaining_steps is sufficient to fully charge and clean path's target
                    charging_cap = max_battery;
                    res = Step::Stay;
                } 
                else{ //remaining_steps is insufficient to fully charge and clean path's target
                    
                    if (remaining_steps -1 >= cleaning_duty_min_steps + stepsNumberToCharge(cleaning_duty_min_steps)){ //We can partly charge and still be able to clean path's target
                        charging_cap = std::min(max_battery, (remaining_steps - 1)/2); 
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
        We will construct our next path to a cleanable/unexplored cell, or to the docking station If there isn't any.
        */
        else{ 
            path = constructNextPath(limiting_factor);
            res = marchTheNextStepOfThePath();
        }
    }
    remaining_steps -=1;
    return res;
}
