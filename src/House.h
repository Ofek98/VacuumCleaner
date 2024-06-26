#ifndef HOUSE_H
#define HOUSE_H

#include "common.h"
#include <stdexcept>
#include <fstream>
#include <vector>
#include <string>


class House {
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
    public:    
        std::vector<Tile> vec;
        size_t dim_x = 0, dim_y = 0;

        Matrix() {};

        Matrix(size_t dim_x, size_t dim_y, bool surround_with_walls=false);
        Tile& operator()(size_t x, size_t y);
        Tile& operator()(Coords location);
        int getDimX()
        {
            return dim_x;
        }
    };

    Matrix tiles;
    House(Matrix tiles);
    int getDirtLevel(Coords location);
    void cleanOnce(Coords location);
    bool isWall(Coords location);
};

#endif // HOUSE_H
