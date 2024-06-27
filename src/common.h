#ifndef COMMON_H
#define COMMON_H

#include <stddef.h>
#include <vector>

class Coords {
public:
    int x;
    int y;

    Coords(int x = 0, int y = 0);
    
    Coords reverse() const;
    bool operator==(const Coords& other) const;
    Coords operator+(const Coords& other) const;
};

#define DIFFLOCATION Coords(0,0)
#define NO_DIRECTION Coords(-1,-1)

enum StepType {
    CLEAN,
    MOVE,
    CHARGE,
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
