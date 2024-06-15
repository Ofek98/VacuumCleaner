#ifndef HOUSE_H
#define HOUSE_H

#include "common.h"

class House {
public:
    int getDirtLevel(Location location);
    void clean(Location location);
    bool isWall(Location location);
};

#endif // HOUSE_H
