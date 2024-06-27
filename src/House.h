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

        class ElementProxy {
            Matrix& mat;
            size_t x, y;
        public:
            ElementProxy(Matrix& mat, size_t x, size_t y);
            Tile operator=(Tile value);
            Tile operator=(const ElementProxy& e);
            operator Tile() const;
            int getStatus() const;
            void cleanOnce();
            bool isWall() const;
        };

    public:         
        Matrix() {};
        Matrix(size_t dim_x, size_t dim_y, bool surround_with_walls=false);
        ElementProxy operator()(size_t x, size_t y);
        ElementProxy operator()(Coords location);
        Tile operator()(size_t x, size_t y) const;
        Tile operator()(Coords location) const;
        size_t getDimX() const;
        size_t getDimY() const;
    };

    Matrix tiles;
    House(Matrix tiles, Coords docking_station);
    size_t getDirtLevel(Coords location) const;
    void cleanOnce(Coords location);
    bool isWall(Coords location) const;
    size_t getTotalDirt() const;
    Coords getDockingStationCoords() const;
    
};

#endif // HOUSE_H
