#ifndef COMMON_H
#define COMMON_H

#include <stddef.h>

class Coords {
public:
    size_t x;
    size_t y;

    Coords(size_t x = 0, size_t y = 0);
    
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
    Coords cords;
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
