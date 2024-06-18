#ifndef COMMON_H
#define COMMON_H

class Cords {
public:
    int x;
    int y;

    Cords(int x = 0, int y = 0);
    
    bool operator==(const Cords& other) const;
    Cords operator+(const Cords& other) const;
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
    Cords cords;
};

const Cords DIRECTIONS[4] = {
    {0, 1},
    {1, 0},
    {0, -1},
    {-1, 0}
};

constexpr int WALL = -1;

#endif // COMMON_H
