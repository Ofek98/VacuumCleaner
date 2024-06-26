#ifndef COMMON_H
#define COMMON_H

#include <stddef.h>

class Coords {
public:
    int x;
    int y;

    Coords(int x = 0, int y = 0);
    
    bool operator==(const Coords& other) const;
    Coords operator+(const Coords& other) const;
};

#define DIFFLOCATION Cords(0,0)

enum StepType {
    CLEAN,
    MOVE,
    CHARGE,
    RETURN,
};


struct Step {
    StepType type;
    Coords coords;
};

const Coords DIRECTIONS[4] = {
    {0, 1},
    {1, 0},
    {0, -1},
    {-1, 0}
};

constexpr int WALL = -1;
constexpr int DOCKING_STATION = -2;

#endif // COMMON_H
