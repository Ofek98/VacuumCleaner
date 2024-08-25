
#ifndef COMMON_H
#define COMMON_H

/** @file common.h
 * @brief This file contains the declaration of the Coords class, StepType enum, Step struct, and other constants and variables used in the vacuum cleaner program.
 */

#include "../common/enums.h"
#include <stddef.h>
#include <vector>
#include <iostream>
#include <functional>

/**
 * @brief The Coords class represents the coordinates of a tile in a matrix of tiles.
 */
class Coords {
public:
    int x; /**< The x-coordinate of the tile. */
    int y; /**< The y-coordinate of the tile. */

    /**
     * @brief Constructs a Coords object with the given x and y coordinates.
     * @param x The x-coordinate.
     * @param y The y-coordinate.
     */
    Coords(int x = 0, int y = 0);
    
    /**
     * @brief Returns the reverse of the current Coords object.
     * @return The reverse of the current Coords object: (-x, -y).
     */
    Coords reverse() const;
    
    /**
     * @brief Checks if the current Coords object is equal to another Coords object.
     * @param other The other Coords object to compare with.
     * @return True if the two Coords objects are equal, false otherwise.
     */
    bool operator==(const Coords& other) const;
    
    /**
     * @brief Adds two Coords objects together.
     * @param other The other Coords object to add.
     * @return The sum of the two Coords objects: (x1+x2, y1+y2).
     */
    Coords operator+(const Coords& other) const;

    Coords operator+(Direction d) const;

    Coords operator-(const Coords& other) const;

    Coords operator+=(const Coords& other);

    Coords operator+=(Direction d);

    friend std::ostream& operator<<(std::ostream& os, const Coords& coords);

    operator Step() const;

};

std::ostream& operator<<(std::ostream& os, const Step& step);


std::ostream& operator<<(std::ostream& os, const std::vector<Coords>& vec);

namespace std {
    template <>
    struct hash<Coords> {
        std::size_t operator()(const Coords& c) const noexcept;
    };
}

using CoordsVector = std::vector<Coords>;
constexpr float WALL = -1; /**< A constant representing a wall. */
constexpr int DOCKING_STATION = -2; /**< A constant representing a docking station. */
const float UNEXPLORED = 0.5; /**< A constant representing a location that wasn't explored yet by the algorithm. */

#endif // COMMON_H