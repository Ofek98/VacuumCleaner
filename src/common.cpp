#include "common.h"

Coords::Coords(int x, int y) : x(x), y(y) {}

bool Coords::operator==(const Coords& other) const {
    return (x == other.x && y == other.y);
}

Coords Coords::operator+(const Coords& other) const {
    return Coords(x + other.x, y + other.y);
}
