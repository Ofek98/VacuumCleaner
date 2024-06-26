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
    
    default:
        if('0' <= status_char && status_char <= '9')
        {
            status = status_char - '0';
        }
        else
        {
            throw std::runtime_error("Cannot initialize tile from char " + status_char);
        }
        break;
    }
}

int House::Tile::getStatus() const
{
    return status;
}

void House::Tile::cleanOnce()
{
    if(status > 0)
        status--;
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

House::Matrix::Matrix(size_t dim_x, size_t dim_y, bool surround_with_walls=false) : dim_x(dim_x + surround_with_walls), dim_y(dim_y + surround_with_walls)
{
    vec.resize((dim_x)*(dim_y), Tile(0));
    if(surround_with_walls) 
        surroundWithWalls();
}

House::Tile& House::Matrix::operator()(size_t x, size_t y)
{
    if (x >= dim_x || y >= dim_y)
        throw std::out_of_range("matrix indices out of range");
    return vec[dim_x*y + x];
}

House::Tile& House::Matrix::operator()(Coords location)
{
    return (*this)(location.x, location.y);
}

House::House(Matrix tiles): tiles(tiles), docking_station(tiles.docking_station) {}

size_t House::getDirtLevel(Coords location)
{
    int status = tiles(location).getStatus();
    return status >= 0 ? status : 0;
}
void House::cleanOnce(Coords location)
{
    tiles(location).cleanOnce();
}
bool House::isWall(Coords location)
{
    return tiles(location).isWall();
}