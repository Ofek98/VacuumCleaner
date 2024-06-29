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

std::string stepTypeToString(StepType step_type) {
    switch (step_type) {
        case StepType::CLEAN: return "CLEAN";
        case StepType::MOVE: return "MOVE";
        case StepType::CHARGE: return "CHARGE";
        default: return "Unknown";
    }
}
