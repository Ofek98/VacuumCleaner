#include "common.h"

 /* @file common.cpp
 * @brief Implementation of the common functions and classes used in the Vacuum Cleaner project.
 */

Coords::Coords(int x, int y) : x(x), y(y) {}

Coords Coords::reverse() const {
    return Coords(-x, -y);
}

bool Coords::operator==(const Coords& other) const {
    return (x == other.x && y == other.y);
}

Coords Coords::operator+(const Coords& other) const {
    return Coords(x + other.x, y + other.y);
}

Coords Coords::operator+(Direction d) const {
    switch (d)
    {
    case Direction::North:
        return Coords(x-1, y);
    
    case Direction::South:
        return Coords(x+1, y);
    
    case Direction::East:
        return Coords(x, y+1);

    case Direction::West:
        return Coords(x, y-1);

    default:
        // we would never reach this line
        return Coords(-1, -1);
    }
}

Coords Coords::operator+=(const Coords& other) {
    Coords sum = *this + other;
    x = sum.x;
    y = sum.y;
    return *this;
}

Coords Coords::operator+=(Direction d) {
    Coords sum = *this + d;
    x = sum.x;
    y = sum.y;
    return *this;
}

Coords::operator Step() const {
    if (x == -1 && y == 0) return Step::North;
    else if (x == 1 && y == 0) return Step::South;
    else if (x == 0 && y == 1) return Step::East;
    else if (x == 0 && y == -1) return Step::West;
    else return Step::Stay; // Default case
}

Coords Coords::operator-(const Coords& other) const {
    return Coords(x - other.x, y - other.y);
}

namespace std {
    std::size_t hash<Coords>::operator()(const Coords& c) const noexcept {
        return std::hash<int>()(c.x) ^ std::hash<int>()(c.y);
    }
}
