Contributors:
Tomer Katee
Ofek Kasif

Our main approach was to learn the map in every step and utilize it using BFS, which gives shortest paths.
The algorithm holds a mapping between locations in the house and their status (possible statuses: wall, unexplored cell, docking station, or a dirt level).
Using the mapping and applying the wall sensor and the dirt sensor in every step, at any given moment the algorithm knows the dirt level in every cell he visited (he also updates the mapping after cleaning), and if its neighbors are explorable or not. 
The algorithm sets paths to the robot's next destination, and every time the robot completes its task at the end of the path (cleaning or charging), the algorithm constructs a new path for it.
The path is determined by BFS, we perform two BFS calls, one gets the path from the current location to the docking, so the robot will know what its the exact distance of the robot from the docking station, and the other is to the closest cleanable or unexplored cell (If there are  some at the same distance, it picks the dirtiest that he knows).
We need to keep in mind that the second BFS call must give a path that allows the robot to reach its destination, explore or clean it, and return to the docking station
For that, the algorithm sets the max path length for the call, which is determined by a limiting factor.
The max path length for an explore/clean duty is the limiting factor/2 (so the robot will be able to reach back its location within the limiting factor steps frame). 
The algorithm utilizes the first BFS call to know the distance from the docking and sets the limiting factor of the second BFS call to be (min(battery left, steps left) - distance to docking) so that the robot will have time to return to the docking station after the duty ends.
When there is no explorable or cleanable cell available within the limiting factor steps frame, the algorithm with set up the first BFS result as the path, sending the robot for charging at the docking station.
When the robot arrives at the docking station at the end of a path. The algorithm checks If there's a possible cleaning duty (assuming full battery). 
If so, the algorithm checks if he can fully charge the robot before going to the task, if not, but it's possible - he will charge it enough to go to the duty and return to the docking station to finish, but not fully charge. If even this is impossible, the robot will finish.
If there's no such duty, the robot will finish as well. 
