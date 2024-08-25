Contributors:
Tomer Katee
Ofek Kasif

Firstly, We have enhanced the algorithm from Assignment 2, making it more efficient at cleaning the house and improving the clarity of the code.

Our main approach was to learn the map in every step and utilize it using BFS, which gives shortest paths.
The algorithm holds a mapping between locations in the house and their status (possible statuses: wall, unexplored cell, docking station, or a dirt level).
Using the mapping and applying the wall sensor and the dirt sensor in every step, at any given moment the algorithm knows the dirt level in every cell he visited (he also updates the mapping after cleaning), and if its neighbors are explorable or not. Plus, in this assignment, the robot also maintained the distances from the docking station to each cell, and the path from the docking station to each cell. The fact that the algorithm knows all the time what are the distances from the docking station to the other cells, helped us to find accessible cleanable cells. While in the previous assignment, the algorithm looked for a cleanable cell in a distance that will alow the robot to go to the cleanable cell, return to the current cell, and return from the current cell to the docking station, this assignment's algorithm looked for a cleanable cell that the robot can reach, and just return directly from the cleanable cell to the docking station in time.
The algorithm constructs a path to the robot's next destination using BFS. If there's a cleanable or unexplored cell that the robot can reach directly and return to the docking station in time, it will do so. Otherwise, a path to the docking station will be constructed, so the robot could charge there or finish.
For the old algorithm, every time the robot completed its task at the end of the path (cleaning or charging), the algorithm constructed a new path for it. For this one, it still creates a new path whenever the algorithm finished its task, but for a change - it also checks if more efficient path can be created at any step where a new cell at the house got revealed (so shorter paths might be created now).
When the robot arrives at the docking station at the end of a path. The algorithm checks If there's a possible cleaning duty (assuming full battery). 
If so, the algorithm checks if it can fully charge the robot before going to the task, if not, but it's possible - he will charge it enough to go to the duty and return to the docking station to finish, but not fully charge. If even this is impossible, the robot will finish.
If there's no such duty, the robot will finish as well. 

Secondly, we've created two variations of this improved algorithm, aiming to cover a wider range of houses where the robot can clean very efficiently.
The variants were based on determinism - one algorithm chose its paths deterministically and the other didn't. 
The randomness worked as follows: The BFS was utilized in two ways. First, to create the shortest paths from the docking station to all cells, and second, to search for a path from the current node to another node that can be cleaned.
When updating paths from the docking station to the other cells, the deterministic version always chooses the same shortest paths in every run, whereas the random version may select different shortest paths to the same node in different runs.
When searching for a cleanable node to navigate to, the target node is selected based on being the closest cleanable node. If there are multiple such nodes, the dirtiest one is chosen. If there is still a tie, the deterministic algorithm always picks the same node, while the random algorithm randomly selects one of these nodes.
Additionally, the path to the target node is chosen deterministically by the deterministic algorithm, while the random algorithm randomly selects one of the shortest paths to the target node in each run.