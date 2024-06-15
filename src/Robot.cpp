#include "Robot.h"

Robot::Robot(House& house, int battery_capacity,Location docking_station)
    : house(house), battery_capacity(battery_capacity),location(docking_station),battery_left(battery_capacity), is_charging(false),algo(*this){
    steps_taken = std::vector<Step>();
    docking_station_path = std::vector<Location>({location});
}

Step Robot::performNextStep(){
    Step step = algo.decide_next_step();
    switch (step.type){
        case CLEAN:
            clean();
            break;
        case MOVE:
            move(step.location, false);
            break;
        case CHARGE:
            charge();
            break;
        case RETURN:
            move(step.location, true);
            break;
    }
    steps_taken.push_back(step);
    return step;
}

void Robot::charge() {
    is_charging = true;
    battery_left = std::max(battery_capacity, battery_left +battery_capacity/20);
}

void Robot::clean() {
    house.clean(location);
}

int Robot::getCurrentLocationDirt(){
    return house.getDirtLevel(location);
}

std::vector<Location> Robot::getSurroundingWalls() {
    std::vector<Location> surrounding_walls;
    
    for (int i = 0; i < 4; ++i) {
        Location new_location = {{location.coords[0] + directions[i][0], location.coords[1] + directions[i][1]}};
        if (!house.isWall(new_location)) {
            surrounding_walls.push_back(new_location);
        }
    }
    
    return surrounding_walls;
}


void Robot::move(Location next_loc, bool is_returning) {
    if (is_returning) {
        docking_station_path.pop_back();
    } else {
        docking_station_path.push_back(location);
    }
    location = next_loc;
}

Location Robot::getLocation() {
    return location;
}
