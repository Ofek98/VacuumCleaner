Contributors to this project:

Tomer Katee
Ofek Kasif

Approach and Solution:
Input Format: The system reads input from a .txt file where:
The first row contains max_battery_steps (an integer).
The second row contains total_steps (an integer).
Subsequent rows form a matrix (n x m) representing the house layout. Each row contains characters {0-9, W, D, SPACE} where digits represent dirt levels, W is a wall, D is a docking station (ensured to be single), and SPACE denotes a clean space.

Design Considerations:

Class Structure: Utilized myrobot.cpp for input/output handling and employed separate classes (Simulator, House, Robot, Algorithm) for distinct functionalities.
Sensors: Modeled sensors as public functions within the Robot class for simplicity and reduced redundancy.

Tiles Representation: Opted for a 2D vector (Matrix class) over hash maps to leverage spatial locality for efficient tile access.
Steps and Coordinates: Defined StepType enum (CLEAN, MOVE, CHARGE) and Coords class for managing coordinates and operations.

Algorithm: Initially considered random movement but settled on a hybrid approach where the robot moves randomly until encountering obstacles or dirt, ensuring efficiency in exploration and cleaning.

Output Format: The output file includes:
A list of steps taken with coordinates and step type (CLEAN, MOVE, CHARGE).
Total steps performed, remaining battery, remaining dirt in the house, and robot status (docking station or not).


Explained more elaborately in the hld.pdf file.
Full documentation website in docs/html/index.html.
