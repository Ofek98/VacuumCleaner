
#ifndef COMMON_H
#define COMMON_H

/** @file common.h
 * @brief This file contains the declaration of the Coords class, StepType enum, Step struct, and other constants and variables used in the vacuum cleaner program.
 */

#include <stddef.h>
#include <vector>
#include <iostream>

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
};

#define DIFFLOCATION Coords(0,0) /**< A constant representing a zero difference in location. */
#define NO_DIRECTION Coords(-1,-1) /**< A constant representing no direction (for edge cases). */

/**
 * @brief The StepType enum represents the type of a step.
 */
enum StepType {
    CLEAN, /**< Represents a cleaning step. */
    MOVE, /**< Represents a movement step. */
    CHARGE, /**< Represents a charging step. */
};


/**
 * @brief Converts a StepType enum value to its corresponding string representation.
 * @param step_type The StepType enum value to convert.
 * @return The string representation of the StepType enum value.
 */
std::string stepTypeToString(StepType step_type);

/**
 * @brief The Step struct represents a step in a cleaning process.
 */
struct Step {
    StepType type; /**< The type of the step. */
    Coords coords; /**< The coordinates associated with the step. */
};

/**
 * @brief An array of Coords objects representing the four cardinal directions: up, right, down, and left.
 */
const Coords DIRECTIONS[4] = {
    {0, 1}, /**< Up direction. */
    {1, 0}, /**< Right direction. */
    {0, -1}, /**< Down direction. */
    {-1, 0} /**< Left direction. */
};

constexpr int WALL = -1; /**< A constant representing a wall. */
constexpr int DOCKING_STATION = -2; /**< A constant representing a docking station. */

#endif // COMMON_H