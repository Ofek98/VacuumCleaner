#include "Robot.h"

Robot::Robot(House& house, size_t battery_capacity)
    : house(house), location(house.getDockingStationCoords()), battery_capacity(battery_capacity),
      battery_left(battery_capacity), algo(this) {}

Step Robot::performNextStep(){
    Step step = algo.decide_next_step();
    Coords dir = step.coords;
    Coords next_loc = location + dir;
    switch (step.type){
        case CLEAN:
            clean();
            break;
        case MOVE:
            move(next_loc);
            break;
        case CHARGE:
            charge();
            break;
    }
    step.coords = next_loc;
    steps_taken.push_back(step);
    return step;
}

void Robot::charge() {
    battery_left = std::max((float)battery_capacity, battery_left + battery_capacity/20);
}

void Robot::clean() {
    house.cleanOnce(location);
    decreaseBattery();
}

size_t Robot::getCurrentCoordsDirt() const{
    return house.getDirtLevel(location);
}

bool* Robot::getSurroundingWalls() {
    static bool walls[4]; 
    for (int i = 0; i < 4; i++) {
        walls[i] = house.isWall(location + DIRECTIONS[i]);  
    }
    return walls;
}

void Robot::move(Coords next_loc) {
    location = next_loc;
    decreaseBattery();
}

float Robot::decreaseBattery() {
    return battery_left >= 1 ? battery_left-- : -1; 
}

float Robot::getBatteryLeft() const {
    return battery_left;
}

std::vector<Step> Robot::getStepsTaken() {
    return steps_taken;
}