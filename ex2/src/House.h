#ifndef HOUSE_H
#define HOUSE_H

/**
 * @file House.h
 * @brief This file contains the declaration of the House class.
 */

#include <stdexcept>
#include <fstream>
#include <string>
#include "common.h"

/**
 * @brief The House class represents a house with tiles that can be cleaned by a vacuum cleaner.
 */
class House {

public:

    /**
     * @brief The Tile class represents a single tile in the house.
     */
    class Tile {
        int status = 0;
    public:
        /**
         * @brief Constructs a Tile object with the given status.
         * @param status The status of the tile.
         */
        Tile(int status);

        /**
         * @brief Constructs a Tile object by the given status character.
         * @param status_char The status character of the tile.
         */
        Tile(char status_char);

        /**
         * @brief Gets the status of the tile.
         * @return The status of the tile.
         */
        int getStatus() const;

        /**
         * @brief Cleans the tile once.
         * @return True if the tile was successfully cleaned, false otherwise (if it was clean already).
         */
        bool cleanOnce();

        /**
         * @brief Checks if the tile is a wall.
         * @return True if the tile is a wall, false otherwise.
         */
        bool isWall() const;
    };

    /**
     * @brief The Matrix class represents a matrix of tiles in the house.
     */
    class Matrix
    {
        std::vector<Tile> vec;
        size_t dim_x;
        size_t dim_y;

        /**
         * @brief Surrounds the matrix with walls.
         */
        void surroundWithWalls();

        /**
         * @brief The ElementProxy class represents a proxy object for accessing elements in the matrix.
         */
        class ElementProxy {
            Matrix& mat;
            size_t x, y;
        public:
            /**
             * @brief Constructs an ElementProxy object with the given matrix and coordinates.
             * @param mat The matrix.
             * @param x The x-coordinate.
             * @param y The y-coordinate.
             */
            ElementProxy(Matrix& mat, size_t x, size_t y);

            /**
             * @brief Assigns a value to the element.
             * @param value The value to assign.
             * @return The assigned value.
             */
            Tile operator=(Tile value);

            /**
             * @brief Assigns the value of another ElementProxy object to the element.
             * @param e The ElementProxy object to assign.
             * @return The assigned value.
             */
            Tile operator=(const ElementProxy& e);

            /**
             * @brief Converts the ElementProxy object to a Tile object.
             * @return The Tile object.
             */
            operator Tile() const;

            /**
             * @brief Gets the status of the element.
             * @return The status of the element.
             */
            int getStatus() const;

            /**
             * @brief Cleans the element once.
             * @return True if the element was successfully cleaned, false otherwise (if it was clean already).
             */
            bool cleanOnce();

            /**
             * @brief Checks if the element is a wall.
             * @return True if the element is a wall, false otherwise.
             */
            bool isWall() const;
        };

    public:
        /**
         * @brief Constructs an empty Matrix object.
         */
        Matrix() {};

        /**
         * @brief Constructs a Matrix object with the given dimensions.
         * @param dim_x The number of columns.
         * @param dim_y The number of rows.
         */
        Matrix(size_t dim_x, size_t dim_y);

        /**
         * @brief Accesses the element at the given coordinates.
         * @param x The x-coordinate.
         * @param y The y-coordinate.
         * @return The ElementProxy object representing the element.
         */
        ElementProxy operator()(size_t x, size_t y);

        /**
         * @brief Accesses the element at the given coordinates.
         * @param location The coordinates.
         * @return The ElementProxy object representing the element.
         */
        ElementProxy operator()(Coords location);

        /**
         * @brief Gets the value of the element at the given coordinates.
         * @param x The x-coordinate.
         * @param y The y-coordinate.
         * @return The value of the element.
         */
        Tile operator()(size_t x, size_t y) const;

        /**
         * @brief Gets the value of the element at the given coordinates.
         * @param location The coordinates.
         * @return The value of the element.
         */
        Tile operator()(Coords location) const;

        /**
         * @brief Gets the number of columns in the matrix.
         * @return The number of columns.
         */
        size_t getDimX() const;

        /**
         * @brief Gets the number of rows in the matrix.
         * @return The number of rows.
         */
        size_t getDimY() const;

    };


    House() {};

    /**
     * @brief Constructs a House object with the given matrix of tiles, docking station coordinates, and total dirt level.
     * @param tiles The matrix of tiles representing the house.
     * @param docking_station The coordinates of the docking station.
     * @param total_dirt The total dirt level of the house.
     */
    House(Matrix tiles, Coords docking_station, size_t total_dirt);

    /**
     * @brief Gets the dirt level at the given location.
     * @param location The coordinates of the location.
     * @return The dirt level at the location.
     */
    size_t getDirtLevel(Coords location) const;

    /**
     * @brief Cleans the tile at the given location once.
     * @param location The coordinates of the location.
     */
    void cleanOnce(Coords location);

    /**
     * @brief Checks if the tile at the given location is a wall.
     * @param location The coordinates of the location.
     * @return True if the tile is a wall, false otherwise.
     */
    bool isWall(Coords location) const;

    /**
     * @brief Gets the total dirt level of the house.
     * @return The total dirt level.
     */
    size_t getTotalDirt() const;

    /**
     * @brief Gets the coordinates of the docking station.
     * @return The coordinates of the docking station.
     */
    Coords getDockingStationCoords() const;

private:
    size_t total_dirt;
    Matrix tiles;
    Coords docking_station;
    
};

#endif //HOUSE_H 

