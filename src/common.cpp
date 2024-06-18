#include "common.h"

Cords::Cords(int x, int y) : x(x), y(y) {}

bool Cords::operator==(const Cords& other) const {
    return (x == other.x && y == other.y);
}

Cords Cords::operator+(const Cords& other) const {
    return Cords(x + other.x, y + other.y);
}
