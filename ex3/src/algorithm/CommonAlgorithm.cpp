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
    dist_from_docking = 0;
    is_dist_from_docking_updated = true;
}
/**
Updating curr_loc maping in coords_info to its real level,
and adding its neighbors if needed (we can only know if they're walls or explorable) 
*/
void CommonAlgorithm::updateDetailsAboutCurrLocAndItsNeighbors(){
        coords_info[curr_loc] = curr_loc == Coords(0, 0)? DOCKING_STATION : dirt_sensor->dirtLevel();
        for(int i = 0; i < 4; i++){
            Direction dir = static_cast<Direction>(i);
            Coords loc = curr_loc + dir;
            if (coords_info.find(loc) == coords_info.end()){ 
                coords_info[loc] = wall_sensor->isWall(dir)? WALL : UNEXPLORED;
            }
        }
}

void CommonAlgorithm::updateDistFromDocking(int dist){
    dist_from_docking = dist;
    is_dist_from_docking_updated = true;
}

/*
Adds the relevant neighbors of current to the queue and adds their mapping in parents map. 
A neighbor is relevant if we know it can be accessed, and we didn't travel it yet during the bfs run.
It returns if we can finish our bfs run, which means:
If to_docking - we can generate a path to the docking station. 
If not - we can generate a path to an unexplored or a cleanable cell.
*/
bool CommonAlgorithm::appendNeighbors(const Coords& current, std::deque<Coords>& queue,std::unordered_map<Coords,Coords> &parents, bool to_docking){
    bool can_finish = false;
    for(int i = 0; i < 4; i++){
        Direction dir = static_cast<Direction>(i);
        Coords neighbor = current + dir;

        if (parents.find(neighbor) == parents.end() && coords_info.find(neighbor) != coords_info.end() && coords_info[neighbor] != WALL){
            //We haven't visited neighbor, it is a known cell which is not a wall

            parents[neighbor] = current;
            queue.push_back(neighbor);

            if (to_docking){
                if (neighbor == Coords(0,0)){ //We found the docking so we can return immediately
                    can_finish = true;
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
CoordsVector CommonAlgorithm::bfs(bool to_docking, size_t limiting_factor, bool is_deterministic){
     
    Coords target = Coords(0,0);
    bool can_finish = false;
    int max_iterations = to_docking? limiting_factor : ((limiting_factor-(curr_loc == Coords(0,0))) / 2);
    /*
    If we just need to return to the docking, we could use all the steps.
    If we want to explore something, We need half of the remaining steps to return.
    If we're starting at the docking station, we want to be able to clean the explored cell, so we assign one step to this.  
    */
    std::deque<Coords> queue = {curr_loc}; 
    std::unordered_map<Coords,Coords> parents;//We will use it to map children to their parents and to remember where we already visited
    parents[curr_loc] = curr_loc; //Arbitrary mapping, we won't use it as we just need to have curr_loc in the parents; 
    for (int i = 0; i < max_iterations; i++){
        size_t len = queue.size();
        if (len == 0){
            //No unknown or dirty cells are reachable
            return {};
        }
        for (size_t j = 0; j < len; j++){ //Popping out all the coords from the current level and adding their relevant neighbors
            Coords current = queue.front();
            queue.pop_front();
            if(appendNeighbors(current, queue, parents, to_docking)){//Adds neighbors to queue and parents if they're not there yet
                //If true - we can finish our run after this iteration
                can_finish = true;
            } 
        }
        if (!is_deterministic){
            /*
            If not deterministic, we shuffle the order of the Coords in the queue.
            This way, we will pick one of the shortest paths randomly. 
            This will also mean that if there are two possible targets with the same dirt level at the same distance, the algorithm will choose one of them at random.
            */

             // Create a random number generator
            std::random_device rd;
            std::mt19937 g(rd());

            // Shuffle the queue
            std::shuffle(queue.begin(), queue.end(), g);
        }
        if (can_finish){
            if (!to_docking) {//Finds the dirtiest known cell in this level and sets it as the target
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
            return createPathByParents(curr_loc,target,parents); 
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
CoordsVector CommonAlgorithm::constructNextPath(size_t limiting_factor, bool is_deterministic) {
    CoordsVector path_to_docking = bfs(true,limiting_factor, is_deterministic);
    CoordsVector path_to_closet_cleanable_cell = bfs(false,limiting_factor-path_to_docking.size(), is_deterministic); 
    //limiting_factor-path_to_docking.size() as limiting_factor so the robot will be able to return to its original place with enough steps to return to the docking after it
    return path_to_closet_cleanable_cell.empty()? path_to_docking : path_to_closet_cleanable_cell;
}

/*
Marches the next step of the path, including needed fields' updates
*/
Step CommonAlgorithm::marchTheNextStepOfThePath(){
    bool in_the_way_to_docking = (path.front() == Coords(0,0));
    Coords next_loc = path.back();
    path.pop_back();
    Step res = Step(next_loc-curr_loc);
    curr_loc = next_loc;
    if (in_the_way_to_docking){
        updateDistFromDocking(path.size()); //This is a shortest path to the docking station so we know we can update it
    }
    else{
        is_dist_from_docking_updated = false; //We can't guarantee real distance from the docking in this case 
    }
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

Step CommonAlgorithm::nextStep(bool is_deterministic){
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
        updateDetailsAboutCurrLocAndItsNeighbors();
    }

    //Now we're deciding the next step

    /*
    Condition 1: curr_loc is cleanable
    */
    if (dirt_sensor->dirtLevel() >= 1){
         
        if (!is_dist_from_docking_updated) { //Make sure dist_from_docking_is_updated
            CoordsVector path_to_docking = bfs(true,limiting_factor, is_deterministic);
            updateDistFromDocking(path_to_docking.size()); //The length of the path to the docking received from bfs 
        }
        if (dist_from_docking + 1 <= limiting_factor){ //Enough steps to clean and return to the docking station
            coords_info[curr_loc] -= 1; //The robot cleans the cell
            remaining_steps -=1; 
            return Step::Stay;
        }
    }

    /*
    Condition 2: Curr_loc is not cleanable, and there is already a path that the algo constructed,
    so the robot will march another step in this path towards its destination.
    Note this is not an else condition regarding to condition 1, because it is possible
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
            
            if (is_charging_cap_updated){ //If we know how much we need to charge
                 
                if(battery_meter->getBatteryState() >= charging_cap){ //We charged enough 
                     
                    is_charging_cap_updated = false;
                    path = bfs(false,limiting_factor, is_deterministic); //Calculate next path
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
                 
                CoordsVector optional_path = bfs(false, std::min(max_battery,remaining_steps-1),is_deterministic); //Checks the shortest path available, assuming battery is not limiting us 
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
            path = constructNextPath(limiting_factor, is_deterministic);
            res = marchTheNextStepOfThePath();
        }
    }
    remaining_steps -=1;
    return res;
}
