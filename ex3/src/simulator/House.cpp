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
            status = '0';
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


House::Matrix::Matrix(size_t dim_x, size_t dim_y) : dim_x(dim_x), dim_y(dim_y) {
    vec.resize(this->dim_x * this->dim_y, Tile(0));
}

House::Matrix::ElementProxy::ElementProxy(Matrix& mat, size_t x, size_t y): mat(mat), x(x), y(y) {}

House::Tile House::Matrix::ElementProxy::operator=(Tile value){
    if (x < mat.getDimX() && y < mat.getDimY())
        mat.vec[mat.getDimX()*y + x] = value;
    return value;
}

House::Tile House::Matrix::ElementProxy::operator=(const ElementProxy& e){
    return *this = static_cast<Tile>(e);
}

House::Matrix::ElementProxy::operator Tile() const {
    if (x < mat.getDimX() && y < mat.getDimY())
        return mat.vec[mat.getDimX()*y + x];
    return Tile('W');
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
    return {*this, x, y}; 
}

House::Matrix::ElementProxy House::Matrix::operator()(Coords location) {
    return (*this)(location.x, location.y);
}

House::Tile House::Matrix::operator()(size_t x, size_t y) const {
    if (x < dim_x && y < dim_y)
        return vec[getDimX()*y + x]; 
    return Tile('W');
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
    return location.x < 0 || location.y < 0 || tiles(location).isWall();
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

void House::setTiles(Matrix&& tiles) {
    this->tiles = tiles;
}

void House::setDockingStation(Coords docking_station) {
    this->docking_station = docking_station;
}

void House::setTotalDirt(size_t total_dirt) {
    this->total_dirt = total_dirt;
}