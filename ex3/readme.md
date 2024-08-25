Contributors:
Tomer Katee
Ofek Kasif

PLEASE NOTE Known_Issues file and bonus.txt

Folders:
algorithm_A:
    contains the implementation of the first (non-deterministic) algorithm.
algorithm_B:
    contains the implementation of the second (deterministic) algorithm.
common:
    exactly equals the 'common' folder in the project's skeleton provided to us.
common_algo_sim:
    includes a file that's common for both the algorithm and simulator (mainly helper classes like Coords)
common_algorithm:
    contains an implemntation of the common part between both our algorithm (preventing code repetition)
simulator:
    contains the main (myrobot) program, as well as Simulator class and House class (and Registrar of course)

Makefiles and building:
The folders: algorithm_A, algorithm_B, simulator, each contains its own Makefile (activated simply by calling make), but the whole project can be built by using the Makefile that's in the main directory of the project (it simply triggers the other smaller Makefiles) by calling make.

Running the project:
Our 'main' program is myrobot, just build and run as follows (from the main directory of the project):
./simulator/myrobot <params>

 
Algorithm Design:

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


Threading Design:

We implemented the multithreading concept by creating 'num_threads' threads in main() that each runs run_simulations() separately.
In order to synchronize the completion of House-Algorithm tasks we set up an atomic task counter called 'counter'.
Each thread runs in a loop that increments counter and takes the incremented value of it as a current task identifier.
Then the algorithm&house combination is derived from the int identifer taken in a way that ensures that each combination is run exactly once by some thread.


Timeout handling:

When an algorithm&house runs for too long, we implemented a timeout mechanism to deal with that:
First, if we return from algorithm next_step() call and find that we run for too long, we simply cut the run due to timeout.
On the other case, if an algorithm next_step() is "stuck" we handle this by creating a 'backup' thread. This thread is created right before calling simulation.run() on the task thread, and as soon as it starts running it goes to sleep for 'timeout' milliseconds. After waking up, the backup thread checks whether the score for the current task was set. If so, it justs finishes running, else it sets the task score to a default value and calls run_simulations() by itself (thus replacing the original stuck thread). Notice that if the stuck thread returns later, it finds that a default score has been set for its task and finishes.
For that purpose we also added a mutex to avoid a situation where both the backup thread and the original one go on to the next task (due to data race in the current task's score entry).

