/**
 * @file House.cpp
 * @brief Implementation file for the House class.
 * 
 * This file contains the implementation of the House class and its member functions.
 * The House class represents a house with a matrix of tiles, where each tile can have
 * a status indicating the amount of dirt or other properties such as walls or docking stations.
 * The House class provides functions to access and manipulate the tiles in the house.
 */
#include "House.h"

House::Tile::Tile(int status): status(status) {}

House::Tile::Tile(char status_char)
{
    switch (status_char)
    {
    case ' ':
        status = 0;
        break;
    
    case 'W':
        status = WALL;
        break;
    
    case 'D':
        status = DOCKING_STATION;
        break;
    
    default:
        if('0' <= status_char && status_char <= '9')
        {
            status = status_char - '0';
        }
        else
        {
            throw std::runtime_error("Cannot initialize tile from char " + std::string(1, status_char));
        }
        break;
    }
}

int House::Tile::getStatus() const
{
    return status;
}

bool House::Tile::cleanOnce()
{
    if(status > 0) {
        status--;
        return true;
    }
    return false;
}

bool House::Tile::isWall() const
{
    return status == WALL;
}

void House::Matrix::surroundWithWalls() {
    for (size_t x = 0; x < dim_x; x++)
    {
        (*this)(x, 0) = (*this)(x, dim_y-1) = Tile(WALL);
    }
    for (size_t y = 0; y < dim_y; y++)
    {
        (*this)(0, y) = (*this)(dim_x-1, y) = Tile(WALL);
    }
}

House::Matrix::Matrix(size_t dim_x, size_t dim_y) : dim_x(dim_x+2), dim_y(dim_y+2) {
    vec.resize(this->dim_x * this->dim_y, Tile(0));
    surroundWithWalls();
}

House::Matrix::ElementProxy::ElementProxy(Matrix& mat, size_t x, size_t y): mat(mat), x(x), y(y) {}

House::Tile House::Matrix::ElementProxy::operator=(Tile value){
    mat.vec[mat.getDimX()*y + x] = value;
    return value;
}

House::Tile House::Matrix::ElementProxy::operator=(const ElementProxy& e){
    return *this = static_cast<Tile>(e);
}

House::Matrix::ElementProxy::operator Tile() const {
    return mat.vec[mat.getDimX()*y + x];
}

int House::Matrix::ElementProxy::getStatus() const {
    return ((Tile)*this).getStatus();
}

bool House::Matrix::ElementProxy::cleanOnce() {
    return mat.vec[mat.getDimX()*y + x].cleanOnce();
}

bool House::Matrix::ElementProxy::isWall() const {
    return ((Tile)*this).isWall();
}

House::Matrix::ElementProxy House::Matrix::operator()(size_t x, size_t y) {
    if (x >= dim_x || y >= dim_y)
        throw std::out_of_range("matrix indices out of range");
    return {*this, x, y}; 
}

House::Matrix::ElementProxy House::Matrix::operator()(Coords location) {
    return (*this)(location.x, location.y);
}

House::Tile House::Matrix::operator()(size_t x, size_t y) const {
    if (x >= dim_x || y >= dim_y) {
        throw std::out_of_range("matrix indices out of range: " + std::to_string(x) + ", " + std::to_string(y));
    }
    return vec[getDimX()*y + x]; 
}

House::Tile House::Matrix::operator()(Coords location) const {
    return (*this)(location.x, location.y);
}

House::House(Matrix tiles, Coords docking_station, size_t total_dirt): total_dirt(total_dirt), tiles(tiles), docking_station(docking_station) {}

size_t House::getDirtLevel(Coords location) const {
    int status = tiles(location).getStatus();
    return status >= 0 ? status : 0;
}
void House::cleanOnce(Coords location) {
    if(tiles(location).cleanOnce()) {
        total_dirt--;
    }
}
bool House::isWall(Coords location) const {
    return tiles(location).isWall();
}

size_t House::Matrix::getDimX() const {
    return dim_x;
}

size_t House::Matrix::getDimY() const {
    return dim_y;
}

Coords House::getDockingStationCoords() const {
    return docking_station;
}

size_t House::getTotalDirt() const {
    return total_dirt;
}