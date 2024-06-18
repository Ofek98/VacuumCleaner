#include "Robot.h"

Robot::Robot(House& house, int battery_capacity,Cords docking_station)
    : house(house), battery_capacity(battery_capacity), location(docking_station), docking_station(docking_station),
      battery_left(battery_capacity), steps_taken(), docking_station_path({docking_station}), algo(*this) {
    steps_taken = std::vector<Step>();
    docking_station_path = std::vector<Cords>({location});
}

Step Robot::performNextStep(){
    Step step = algo.decide_next_step();
    Cords dir = step.cords();
    Cords next_loc = location + dir;
    switch (step.type){
        case CLEAN:
            clean();
            break;
        case MOVE:
            move(next_loc, false);
            break;
        case CHARGE:
            charge();
            break;
        case RETURN:
            move(next_loc, true);
            break;
    }
    steps_taken.push_back(step);
    return step;
}

void Robot::charge() {
    battery_left = std::max(battery_capacity, battery_left +battery_capacity/20);
}

void Robot::clean() {
    house.clean(location);
    decreaseBattery();
}

int Robot::getCurrentLocationDirt(){
    return house.getDirtLevel(location);
}

int* Robot::getSurroundingWalls() {
    static int walls[4]; 
    for (int i = 0; i < 4; i++) {
        Cords checkLocation = location + DIRECTIONS[i];  
        walls[i] = house.isWall(checkLocation) ? 1 : 0;  
    }
    return walls;
}


void Robot::move(Location next_loc, bool is_returning) {
    if (is_returning) {
        next_loc = docking_station_path.back();
        docking_station_path.pop_back();
        
    } else {
        docking_station_path.push_back(location);
    }
    location = next_loc;
    decreaseBattery();
}

//Location Robot::getLocation() {
//    return location;
//}

float Robot::decreaseBattery() {
    if (battery_left >= 1){
        battery_left -= 1;
        return battery_left
    }
    return -1;
}

float Robot::getBatteryLeft() {
    return battery_left;
}
