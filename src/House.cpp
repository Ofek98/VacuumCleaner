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

House::Matrix::Matrix(size_t dim_x, size_t dim_y) : dim_x(dim_x), dim_y(dim_y)
{
    vec.resize(dim_x*dim_y, Tile(0));
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

House::House(std::ifstream& tiles_file)
{
    std::string line;
    size_t lines_count = 0;
    size_t max_line_length = 0;
    while(std::getline(tiles_file, line))
    {
        if(max_line_length < line.length())
            max_line_length = line.length();
        lines_count++;
    }

    tiles = Matrix(max_line_length, lines_count);

    int x = 0, y = 0;
    while(std::getline(tiles_file, line))
    {
        for (size_t x = 0; x < tiles.getDimX(); x++)
        {
            tiles(x, y) = x < line.length() ? Tile(line[x]) : Tile(0);
        }
        y++;
    }
}

int House::getDirtLevel(Coords location)
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