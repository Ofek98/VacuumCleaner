#ifndef HOUSE_H
#define HOUSE_H

#include "common.h"
#include <map>
#include <fstream>


class House {
    class Tile {
        int status = CLEAN;
        Tile(status);
        int getStatus() const;
        void cleanOnce();
    }

    std::map<Location, Tile> tiles;
public:
    House(std::ifstream tiles_file);
    int getDirtLevel(Location location);
    void clean(Location location);
    bool isWall(Location location);
};

#endif // HOUSE_H
