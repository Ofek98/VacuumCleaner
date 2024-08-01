Contributors to this project:

Tomer Katee
Ofek Kasif

Approach and Solution:

We implement

Class Structure: Utilized myrobot.cpp for input/output handling and employed separate classes (Simulator, House, Robot, Algo_214166027) for distinct functionalities.
Sensors: Modeled sensors as public functions within the Robot class for simplicity and reduced redundancy.

Tiles Representation: Opted for a 2D vector (Matrix class) over hash maps to leverage spatial locality for efficient tile access.
Steps and Coordinates: Defined StepType enum (CLEAN, MOVE, CHARGE) and Coords class for managing coordinates and operations.

Algo_214166027: Initially considered random movement but settled on a hybrid approach where the robot moves randomly until encountering obstacles or dirt, ensuring efficiency in exploration and cleaning.

Output Format: The output file includes:
A list of steps taken with coordinates and step type (CLEAN, MOVE, CHARGE).
Total steps performed, remaining battery, remaining dirt in the house, and robot status (docking station or not).


Explained more elaborately in the hld.pdf file.
Full documentation website in docs/html/index.html.
