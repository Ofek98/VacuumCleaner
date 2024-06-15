#ifndef COMMON_H
#define COMMON_H

struct Location {
    int coords[2];
};

enum StepType {
    CLEAN,
    MOVE,
    CHARGE,
    RETURN,
};

struct Step {
    StepType type;
    Location location;
};
const int DIRECTIONS[4][2] = {
    {0, 1}, 
    {1, 0},  
    {0, -1}, 
    {-1, 0}  
};

constexpr int WALL = 1;

#endif // COMMON_H