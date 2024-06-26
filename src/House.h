#ifndef HOUSE_H
#define HOUSE_H

#include "common.h"
#include <stdexcept>
#include <fstream>
#include <vector>
#include <string>


class House {
    size_t total_dirt;
    Coords docking_station;

public:

    class Tile {
        int status = 0;
    public:
        Tile(int status);
        Tile(char status_char);
        int getStatus() const;
        void cleanOnce();
        bool isWall() const;
    };

    class Matrix
    {
        void surroundWithWalls();
        std::vector<Tile> vec;
        size_t dim_x = 0, dim_y = 0;

    public:    

        Matrix() {};

        Matrix(size_t dim_x, size_t dim_y, bool surround_with_walls=false);
        Tile& operator()(size_t x, size_t y);
        Tile& operator()(Coords location);
        size_t getDimX() const {
            return dim_x;
        }
        size_t getDimY() const {
            return dim_y;
        }
    };

    Matrix tiles;
    House(Matrix tiles);
    size_t getDirtLevel(Coords location);
    void cleanOnce(Coords location);
    bool isWall(Coords location);
    size_t getTotalDirt();
    Coords getDockingStationCoords();
    
};

#endif // HOUSE_H
