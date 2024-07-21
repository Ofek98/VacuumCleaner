/**
 * @file Algorithm.cpp
 * @brief Implementation file for the Algorithm class.
 */
#include "Algorithm.h"
#include <cstdlib>
#include <iostream>


Algorithm::Algorithm(){
    curr_loc = Coords(0, 0);
    is_returning = false;
    has_first_step_taken = false;
}
/*
Updating curr_loc maping in coords_info to its real level,
and adding its neighbors if needed (we can only know if they're walls or explorable) 
*/
void Algorithm::updateDetailsAboutCurrLocAndItsNeighbors(){
        coords_info[curr_loc] = dirt_sensor->dirtLevel();
        for(int i = 0; i < 4; i++){
            Direction dir = static_cast<Direction>(i);
            Coords loc = curr_loc + dir;
            if (coords_info.find(loc) == coords_info.end()){ 
                coords_info[loc] = wall_sensor->isWall(dir)? WALL : UNEXPLORED;
            }
        }
}

/*
Adds the relevant neighbors of current to the queue and adds their mapping in parents map. 
A neighbor is relevant if we know it can be accessed, and we didn't travel it yet during the bfs run.
It returns if we can finish our bfs run, which means:
If its_returning - we can generate a path to the docking station. 
If not - we can generate a path to an unexplored or a cleanable cell.
*/
bool Algorithm::appendNeighbors(const Coords& current, std::deque<Coords>& queue,std::unordered_map<Coords,Coords> parents){
    bool can_finish = false;
    for(int i = 0; i < 4; i++){
        Direction dir = static_cast<Direction>(i);
        Coords neighbor = current + dir;
        if (parents.find(neighbor) != parents.end() && coords_info.find(neighbor) != coords_info.end() && coords_info[neighbor] != WALL){
            //We haven't visited neighbor, it is a known cell which is not a wall
            parents[neighbor] = current;
            queue.push_back(neighbor);
            if (is_returning){
                if (neighbor == Coords(0,0)){
                    can_finish = true;
                    return can_finish;
                }
            }
            else{
                if (coords_info[neighbor] > 0){
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
std::vector<Coords> createPathByParents(Coords start,Coords target,std::unordered_map<Coords,Coords> parents){
    std::vector<Coords> next_path;
    next_path.push_back(target);
    Coords current = target;
    while (parents[current] != start){
        current = parents[current];
        next_path.push_back(current);
    }
    std::reverse(next_path.begin(), next_path.end());
    return std::move(next_path);
}

/*
Calculating the shortest path possible from curr_loc to the target, where the target depends in whether we're returning or not.
If so - the target is the docking station, located in (0,0).
If not - the target is the closest unexplored or cleanable cell.
If is more than one, then the target will be the most dirty known cell.
*/
std::vector<Coords> Algorithm::bfs(){
    Coords target;
    bool can_finish = false;
    size_t max_iterations = is_returning ? limiting_factor : ((limiting_factor-1)/ 2);
    /*
    If we just need to return to the docking, we could use all the steps.
    If we want to clean something, we will need at least one step for the cleaning, and 2 steps X path's length to the dirty cell
    */
    std::deque<Coords> queue = {curr_loc}; 
    std::unordered_map<Coords,Coords> parents;//We will use it to map children to their parents and to remember where we already visited
    parents[curr_loc] = curr_loc; //Arbitrary mapping, we won't use it as we just need to have curr_loc in the parents; 
    if (is_returning){
        target = Coords(0,0);
    }
    for (int i = 1; i < limiting_factor; i++){
        size_t len = queue.size();
        if (len == 0){
            //No unknown or dirty cells are reachable
            return {};
        }
        for (int j = 0; j < len; j++){ //Poping out all the coords from the current level and entering their relevant neighbors
            Coords current = queue.front();
            queue.pop_front();
            can_finish = appendNeighbors(current, queue, parents); 
        }
        if (can_finish){
            if (!is_returning) { //Setting up the target in case of not returning, as explained in the function documentation
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

            std::vector<Coords> next_path = createPathByParents(curr_loc,target,parents);
            return std::move(next_path);
        }
    }
    //No dirty or unknown cell was reachable within limiting_factor steps
    return {};
}
/*
Based on the path updates curr_loc and the next step that will be taken, 
as well as dist_from_docking that's getting updated by whether the robot is returning or not
*/
Step Algorithm::marchTheNextStepOfThePath(){
        dist_from_docking = is_returning? dist_from_docking - 1 : dist_from_docking + 1;
        Coords next_loc = path.back();
        path.pop_back();
        Step res = Step(next_loc-curr_loc);
        curr_loc = next_loc;
        return res;
}

Step Algorithm::nextStep() {
    Step res;
    //Limiting_factor is the actual number of steps until robot must return to the docking_station
    limiting_factor = std::min(remaining_steps, battery_meter->getBatteryState());

    /*
    First we make sure that the current location and its neighbors are updated in coords_info.
    For curr_loc we can know its dirt level, and for the neighbors we can know if they're walls
    */

    if (!has_first_step_taken){
        has_first_step_taken = true;
        coords_info[curr_loc] = DOCKING_STATION;
        updateDetailsAboutCurrLocAndItsNeighbors();
    }
    if (coords_info[curr_loc] == UNEXPLORED) {
        updateDetailsAboutCurrLocAndItsNeighbors();
    }

    //Now we're deciding the next step

    /*
    Condition 1: We're at the docking station
    */
    if (curr_loc == Coords(0,0)){
        dist_from_docking = 0;
        //Condition 1.1: the robot is still charging
        if(battery_meter->getBatteryState() < max_battery){
            res = Step::Stay;
        }
        /*
        Condition 1.2: the robot is fully charge.
        The algorithm will set it a path to the closest cleanable or unknown cell.
        If the path is non empty the algo will command the robot to march the first step in this path, else to finish its run
        */
        else{
            is_returning = false;
            path = bfs();
            if (path.empty()){
                //No more reachable cleanable tiles
                res = Step::Finish;
            }
            else{
                marchTheNextStepOfThePath();
            }
        }
    }

    /*
    Condition 2: curr_loc is cleanable and the limiting_factor is big enough 
    for the robot to return to the docking station safely
    */
    else if (dist_from_docking+1 <= limiting_factor && dirt_sensor->dirtLevel() >= 1){
        res = Step::Stay;
        coords_info[curr_loc] -= 1; 
    }

    /*
    Condition 3: The robot is on its traversal, curr_loc is not cleanable,
    and the known distance from the docking station is too big so that we can't move to another cleanable cell 
    and clean it within the limiting_factor X steps bound
    */
    else if (dist_from_docking + 2 > limiting_factor && is_returning == false){
        is_returning == true;
        path = bfs();
        dist_from_docking = path.size();
        /*If we'll discover that the real distance to the docking is shorter than what we thought it was, according to the bfs result,
        then maybe we could extend the robot's traversal a little bit*/
        if (dist_from_docking + 2 < limiting_factor){
            /*Meaning we can extend the robot's traversal.
            So we calculate the path again, now with is_returning = false. 
            The bfs calculation takes its_returning into account. 
            If so - calculate a path to the docking station, if not - a path to the closest cleanable/unexplored cell*/
            is_returning = false;
            path = bfs();
        }
        res = marchTheNextStepOfThePath(); //The path depends on the flow of the run here
    }

    /*
    Condition 4: There is a path that was setted up by the algorithm
    to the closest cleanable/unexplored cell, so the robot will continue in this path
    */
    else if (!path.empty()){
        //The algorithm already defined a path to the robot, and the robot needs to continue on it 
        dist_from_docking = is_returning? dist_from_docking-1 : dist_from_docking+1; 
        res = marchTheNextStepOfThePath();
    }

    /*Condition 5: The robot reached its path destination (its_returning == false. Otherwise the destination is the docking and we will meet condition 1)
    and its now cleaned it completely. 
    The robot will get a new path to the closest reachable cleanable/unexplored cell. 
    If no such cell  existing, It will return to the docking station
    */
    else{
        path = bfs();
        if (path.empty()){
            is_returning = true;
            path = bfs();
        }
        Step res = marchTheNextStepOfThePath();
    }
    /**The robot is in the middle of its traversal, its path is not empty, 
    the tile it's on is clean and he have enough battery to explore more tiles;*/
    //TODO: calculate next step's direction and go for it; change distance from docking; if not possible, set to return to docking.
    remaining_steps -=1; 
    return res;
}

void Algorithm::setMaxSteps(std::size_t maxSteps) {
    max_steps = maxSteps;
    remaining_steps = maxSteps;
}
void Algorithm::setWallsSensor(const WallsSensor& wallSensor) {
    wall_sensor = &wallSensor;
}
void Algorithm::setDirtSensor(const DirtSensor& dirtSensor) {
    dirt_sensor = &dirtSensor;
}
void Algorithm::setBatteryMeter(const BatteryMeter& batteryMeter) {
    battery_meter = &batteryMeter;
    max_battery = battery_meter->getBatteryState();
}


//TODO: discuss Tomer if we should keep distance_from_docking in return
//TODO: change function name to the convention
//TODO: change to const whatever possible