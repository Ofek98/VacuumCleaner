#ifndef HOUSE_H
#define HOUSE_H

#include "common.h"
#include <stdexcept>
#include <fstream>
#include <vector>
#include <string>


class House {
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
        std::vector<Tile> vec;
        size_t dim_x = 0, dim_y = 0;

    public:
        Matrix() {};

        Matrix(size_t dim_x, size_t dim_y);
        Tile& operator()(size_t x, size_t y);
        Tile& operator()(Coords location);
        int getDimX()
        {
            return dim_x;
        }
    };

    Matrix tiles;
public:
    House(std::ifstream& tiles_file);
    int getDirtLevel(Coords location);
    void cleanOnce(Coords location);
    bool isWall(Coords location);
};

#endif // HOUSE_H
