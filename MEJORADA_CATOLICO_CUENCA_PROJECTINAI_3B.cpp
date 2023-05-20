 //Kurt Angelo B. Mejorada 
 //Karl Catolico
 //Paolo Cuenca

/*! \mainpage Machine Problem
 *
 * \section intro_sec Introduction: BSCS 3B AI PROJECT
 *
 * This project is led by Kurt Mejorada, with Karl Francis Catolico running the machine and finding bugs while Paolo Cuenca was tasked 
 * with documenting and providing analysis to the given in trying to program both blind and heuristic search algorithms.
 *
 * \section instruct_sec Instructions
 * 
 * Write a program in C/C++ implementing a blind search strategy, i.e. Iterative Deepening 
 * Search(IDS) and a heuristic search strategy, i.e. A* Search with graph search to solve the 8-
 * puzzle problem using Manhattan distance as the heuristic. Your program should use the 
 * board configuration below as the goal state and lets the user input the initial/start board 
 * configuration.
 *  |   |   |   |
 *	| -:|:-:|:- |
 *	| 1 | 2 | 3 |
 *	| 8 |   | 4 |
 * 	| 7 | 6 | 5 |
 * 
 * - It should output the following for both the IDS and A* Search:
 *	1. solution path(corresponds to the moves needed to reach the goal): e.g. [Up-Left-Left-Right]
 *  2. solution cost(# of moves in the solution): 4
 *  3. number of nodes expanded before reaching the goal
 *  4. running time
 * 	
 * 
 * \section analys_sec Analysis
 * 
 * \image latex AI1.png width=14cm 
 * 
 *			During the multiple runs executed for both A* and IDS algorithm. It can be inferred that 
 		there was not much difference in configurations for the easy and medium categories but as 
		we moved on to more difficult ones, the time it took for IDS significantly increased. This 
		can be also be observed from the huge difference between the nodes expanded for the two 
		algorithms, thus having longer running times. 
			There were also situations where both algorithms had the same solution cost but IDS 
		would take much longer. From the multiple configurations tested, the results show that 
		A* was better for all these configurations, still the use of IDS 
		would not be much different for lower difficulty configurations of the 8-puzzle. 
 * 
 */

#include <iostream>
#include <time.h>
using namespace std;
#define SIZE 3
/// @note  This is done to prevent weaker machines from crashing as the IDS algorithm runs   
#define MAXDEPTH 12 

/// Creating objects which keeps the position of the blank tile for each state.
class Vector
{
public:
	int i, j;
	void setIndex(int x, int y)
	{
		i = x; /// @param i is the x value
		j = y; /// @param j is the y value
	}
};

/// The main data structure for storing a state.
struct puzzle /// This is in visualization of a puzzle.
{
	int board[SIZE][SIZE];
	Vector blankTile;
	/// Depth of a node used in IDS.
	int level;		
	/// Holds the previous tile movement.	   
	char move;	
	///This is used in astar search.		   
	int manhattanDistance; 
	/// Pointer to the parent node.
	puzzle *parent;		   
};

/// Data structure for creating a linked-list of states.
struct list
{
	puzzle *state;
	list *next;
};

/// Set for global variables.
int goalState[][SIZE] = {{1, 2, 3}, {8, 0, 4}, {7, 6, 5}};

/// function declarations.
puzzle *newState(int state[][SIZE]);
puzzle *newInitialState(int arr[][SIZE]);
puzzle *move(puzzle *state, char direction);
bool movable(puzzle *state, char direction);
bool isEqual(puzzle *state1, puzzle *state2);
bool isGoal(puzzle *state1);
void printState(puzzle *state);
int printStates(puzzle *state);
int getManhattanDistance(puzzle *state);

void Astar(puzzle *initialState);
void IDS(puzzle *initialState);

/// This class manipulates the values of states.
class List
{
public:
	
	list *lst = NULL; // Initializing an empty list of states.
	
	puzzle *front() /// Accessing a node (state) in the BEGINNING of the list & popping it afterwards.
	{
		puzzle *tmp = NULL;
		if (lst != NULL)
		{
			tmp = lst->state;
			list *temp = lst;
			lst = lst->next;
			delete temp;
		}
		return tmp;
	}
	/// Accessing a node (state) in the END of the list & popping it afterwards.
	puzzle *end() 
	{
		if (lst->next == NULL)
		{
			front();
			return lst->state;
		}
		puzzle *tmp = NULL;
		if (lst != NULL)
		{
			list *temp = lst;
			while (temp->next != NULL)
			{
				temp = temp->next;
			}
			tmp = temp->state;
			delete temp;
			temp = NULL;
		}
		return tmp;
	}
	void insertFront(puzzle *s)
	{
		list *tmp = new (list);
		tmp->state = s;
		tmp->next = NULL;
		if (lst != NULL)
		{
			tmp->next = lst;
		}
		lst = tmp;
	}
	void insertEnd(puzzle *s)
	{
		if (lst == NULL)
		{
			insertFront(s);
			return;
		}
		list *temp = lst;
		list *tmp = new (list);
		tmp->state = s;
		tmp->next = NULL;
		while (temp->next != NULL)
		{
			temp = temp->next;
		}
		temp->next = tmp;
	}
	
	
	/// Helps preventing insertion of the same node twice into the list.
	bool notListed(puzzle *state) ///< @returns **FALSE** or **TRUE** if the given state is already in the list or not.
	{
		list *tmplist = lst;
		while (tmplist != NULL)
		{
			if (isEqual(state, tmplist->state))
			{
				return false;
			}
			tmplist = tmplist->next;
		}
		return true;
	}

	puzzle *pickBestState()
	{ /// Chooses the state on the entire list with the lowest heuristic value.
		list *tmplist = lst;
		list *previous;
		list *survivor; // holds the state with the lowest heuristic value.
		puzzle *bestState = NULL;
		int min;

		if (lst->next == NULL)
		{ // CASE 1: has only 1 node in the list, just return that. that's already the bestState since there's no other state to compare with in the first place.
			bestState = lst->state;
			getManhattanDistance(bestState);
			delete lst;
			lst = NULL;
			return bestState;
		}
		// finding the state with lowest hueristic value by looping through the entire list.
		min = getManhattanDistance(tmplist->state); // gets the heuristic value of the first state and sets that value as the minimum.
		survivor = tmplist;							// initially, the best state is set to the first state in the list.
		while (tmplist->next != NULL)
		{
			int dist = getManhattanDistance(tmplist->next->state);
			if (dist < min)
			{
				previous = tmplist;
				survivor = tmplist->next;
				min = dist;
			}
			tmplist = tmplist->next; // iterate.
		}
		bestState = survivor->state;
		if (lst != NULL)
		{
			if (survivor == lst)
			{ // CASE 1: bestState is found at the start of the list.
				return front();
			}
			else if (survivor->next == NULL)
			{						   // CASE 2: bestState is at the last.
				previous->next = NULL; // link the previous state to NULL means removing the last state out of the list.
			}
			else
			{									 // CASE 3: bestState is found in the middle so unlink the middle and reconnect the list.
				previous->next = survivor->next; // link the previous node and the node next to bestState.
			}
		}
		delete survivor;
		return bestState;
	}
	void view()
	{ /// View the entire list of states.
		if (lst != NULL)
		{
			list *tmp = lst;
			while (tmp != NULL)
			{
				cout << getManhattanDistance(tmp->state) << " ";
				tmp = tmp->next;
			}
		}
		else
			cout << "empty list";
	}
};

/// MAIN FUNCTION
int main()
{

	/// Initialization of various difficulties to choose from
	int easy[][SIZE] = {{1, 3, 4}, {8, 6, 2}, {7, 0, 5}};
	int medium[][SIZE] = {{2, 8, 1}, {0, 4, 3}, {7, 6, 5}};
	int hard[][SIZE] = {{2, 8, 1}, {4, 6, 3}, {7, 5, 0}};
	int worst[][SIZE] = {{5, 6, 7}, {4, 0, 8}, {3, 2, 1}};
	int preferred[][SIZE] = {{1, 3, 4}, {8, 6, 2}, {7, 0, 5}};
	int input[SIZE][SIZE] = {0};
	int choice = 0;
	cout << "Choose initial config: \n";
	cout << "1 Easy\n2 Medium\n3 Hard\n4 Worst\n5 Keyboard Input\n";
	cin >> choice;

	puzzle *init = NULL;
	switch (choice)
	{
	case 1:
		init = newInitialState(easy);
		break;
	case 2:
		init = newInitialState(medium);
		break;
	case 3:
		init = newInitialState(hard);
		break;
	case 4:
		init = newInitialState(worst);
		break;
	case 5:
		cout << "-------Initial Board Config-------" << endl;
		cout << "Please input the tile arrangement for each row" << endl
			 << endl;
		for (int i = 0; i < SIZE; i++)
		{
			cout << "ROW" << i + 1;
			"\n";
			for (int j = 0; j < SIZE; j++)
			{
				cin >> input[i][j];
			}
		}
		init = newInitialState(input);
		break;
	default:
		cout << "Invalid input";
		return 0;
	}
	cout << "\n\n";

	if (isEqual(init, newState(easy)))
		cout << "EASY\n";
	else if (isEqual(init, newState(medium)))
		cout << "MEDIUM\n";
	else if (isEqual(init, newState(hard)))
		cout << "HARD\n";
	else if (isEqual(init, newState(worst)))
		cout << "WORST\n";
	else if (isEqual(init, newState(preferred)))
		cout << "PREFERRED\n";
	printState(init);

	double CpuTimeUsed; /// computation for running time
	clock_t start, end;

	cout << "\n-------A* Algorithm---------\nstatus: searching...";
	start = clock();
	Astar(init);
	end = clock();
	CpuTimeUsed = ((double)(end - start)) / CLOCKS_PER_SEC;
	cout << "Running Time: " << CpuTimeUsed << endl;

	cout << "\n\n\n-------IDS Algorithm-------\nstatus: searching...";
	start = clock();
	IDS(init);
	end = clock();
	CpuTimeUsed = ((double)(end - start)) / CLOCKS_PER_SEC;
	cout << "Running Time: " << CpuTimeUsed << endl;
	cout << "\n\n\n";

	return 0;
}

/// A* Search Algorithm
void Astar(puzzle *initialState)
{
	List openList;

	List closedList;

	int counter = 0;

	openList.insertFront(initialState);

	while (openList.lst != NULL)
	{

		puzzle *bestState = openList.pickBestState();
		closedList.insertFront(bestState);

		if (isGoal(bestState))
		{
			cout << endl
				 << "status: Solution Found";
			cout << endl
				 << endl
				 << "Solution Path: ";
			int cost = printStates(bestState);
			cout << "\nNodes Expanded: " << counter;
			cout << "\nSolution Cost: " << cost - 1 << endl;
			return;
		}

		counter++;

		if (movable(bestState, 'U'))
		{
			puzzle *tmp = move(bestState, 'U');
			if (closedList.notListed(tmp))
			{
				openList.insertFront(tmp);
			}
		}

		if (movable(bestState, 'R'))
		{
			puzzle *tmp = move(bestState, 'R');
			if (closedList.notListed(tmp))
			{
				openList.insertFront(tmp);
			}
		}

		if (movable(bestState, 'D'))
		{
			puzzle *tmp = move(bestState, 'D');
			if (closedList.notListed(tmp))
			{
				openList.insertFront(tmp);
			}
		}

		if (movable(bestState, 'L'))
		{
			puzzle *tmp = move(bestState, 'L');
			if (closedList.notListed(tmp))
			{
				openList.insertFront(tmp);
			}
		}
	}
}

/// IDS Search Algorithm
/// @warning This may cause crashing for weaker machines when executed.
void IDS(puzzle *initialState)
{
	int i = 0, counter = 0;
	while (true)
	{
		List closed;
		List stack;
		stack.insertFront(initialState);
		while (stack.lst != NULL)
		{
			puzzle *first = stack.front();

			if (first->level > i)
				continue;

			closed.insertFront(first);

			if (isGoal(first))
			{
				cout << endl
					 << "status: solution found";
				cout << endl
					 << endl
					 << "Solution Path: ";
				int cost = printStates(first);
				cout << "\nNodes expanded: " << counter;
				cout << "\nSolution Cost: " << cost - 1 << endl;
				return;
			}
			counter++;
			if (movable(first, 'U'))
			{
				puzzle *tmp = move(first, 'U');
				if (closed.notListed(tmp))
				{
					stack.insertFront(tmp);
				}
			}
			if (movable(first, 'R'))
			{
				puzzle *tmp = move(first, 'R');
				if (closed.notListed(tmp))
				{
					stack.insertFront(tmp);
				}
			}
			if (movable(first, 'D'))
			{
				puzzle *tmp = move(first, 'D');
				if (closed.notListed(tmp))
				{
					stack.insertFront(tmp);
				}
			}
			if (movable(first, 'L'))
			{
				puzzle *tmp = move(first, 'L');
				if (closed.notListed(tmp))
				{
					stack.insertFront(tmp);
				}
			}
		}
		i++;
	}
}

/// function implementations
puzzle *newState(int state[][SIZE]) /// new state that accepts array that contains tile arrangement
{
	puzzle *tmp = new (puzzle);
	for (int i = 0; i < SIZE; i++)
	{
		for (int j = 0; j < SIZE; j++)
		{
			if (state[i][j] == 0) /// finds the blank tile
				tmp->blankTile.setIndex(i, j);
			tmp->board[i][j] = state[i][j];
			tmp->manhattanDistance = -1; /// by default sets to -1 to determine if it is not yet calculated
		}
	}
	return tmp;
}
	/// the state that accepts array which contains tile arrangement.
	/// initial state has a g(x) = 0.
	/// S = Starting move.
	/// @returns the created state.
puzzle *newInitialState(int arr[][SIZE]) /// creation of new initial state
{

	puzzle *state = newState(arr); 
	state->level = 0;			   
	state->move = 'S';			   
	state->parent = NULL;
	return state; 
}

/// checks the visited list if the state has already been on it -- notListed() function
bool isEqual(puzzle *state1, puzzle *state2)
{
	for (int i = 0; i < SIZE; i++)
	{
		for (int j = 0; j < SIZE; j++)
		{
			if (state1->board[i][j] != state2->board[i][j])
				return false;
		}
	}
	return true;
}

/// checks if the goal has been found through comparison
bool isGoal(puzzle *state1)
{
	for (int i = 0; i < SIZE; i++)
	{
		for (int j = 0; j < SIZE; j++)
		{
			if (state1->board[i][j] != goalState[i][j])
				return false;
		}
	}
	return true;
}

void printState(puzzle *state)
{
	for (int i = 0; i < SIZE; i++)
	{
		for (int j = 0; j < SIZE; j++)
		{
			cout << state->board[i][j] << " | ";
		}
		cout << endl;
	}
}

int printStates(puzzle *state)
{
	if (state != NULL)
	{
		int i;
		i = printStates(state->parent) + 1;
		if (state->move == 'S')
			return i;
		cout << " " << state->move;
		return i;
	}
	else
	{
		return 0;
	}
}

/// movement of the blank tile
puzzle *move(puzzle *state, char direction)
{
	puzzle *tmp = newState(state->board);
	tmp->parent = state;
	tmp->level = state->level + 1;

	/// actions of the blank tile | U = Up; R = Right; D = Down; L = Left
	if (direction == 'U')
	{
		tmp->move = 'U';
		tmp->board[tmp->blankTile.i][tmp->blankTile.j] = tmp->board[tmp->blankTile.i - 1][tmp->blankTile.j];
		tmp->blankTile.i--;
		tmp->board[tmp->blankTile.i][tmp->blankTile.j] = 0;
	}
	else if (direction == 'R')
	{
		tmp->move = 'R';
		tmp->board[tmp->blankTile.i][tmp->blankTile.j] = tmp->board[tmp->blankTile.i][tmp->blankTile.j + 1];
		tmp->blankTile.j++;
		tmp->board[tmp->blankTile.i][tmp->blankTile.j] = 0;
	}
	else if (direction == 'D')
	{
		tmp->move = 'D';
		tmp->board[tmp->blankTile.i][tmp->blankTile.j] = tmp->board[tmp->blankTile.i + 1][tmp->blankTile.j];
		tmp->blankTile.i++;
		tmp->board[tmp->blankTile.i][tmp->blankTile.j] = 0;
	}
	else if (direction == 'L')
	{
		tmp->move = 'L';
		tmp->board[tmp->blankTile.i][tmp->blankTile.j] = tmp->board[tmp->blankTile.i][tmp->blankTile.j - 1];
		tmp->blankTile.j--;
		tmp->board[tmp->blankTile.i][tmp->blankTile.j] = 0;
	}
	else
		return NULL;
	return tmp;
}

/// checks if it is a valid move for the blank tile
bool movable(puzzle *state, char direction)
{
	if (direction == 'U')
	{
		if (state->blankTile.i > 0)
			return true;
	}
	else if (direction == 'R')
	{
		if (state->blankTile.j < SIZE - 1)
			return true;
	}
	else if (direction == 'D')
	{
		if (state->blankTile.i < SIZE - 1)
			return true;
	}
	else if (direction == 'L')
	{
		if (state->blankTile.j > 0)
			return true;
	}
	return false;
}

/// @return the absolute value of the integer | abs = absolute
int abs(int x)
{
	if (x < 0)
	{
		x -= (x * 2);
	}
	return x;
}

/// Used in getManhattanDistance function
int distBetween2Tiles(puzzle *state, Vector correctTile)
{
	for (int y = 0; y < SIZE; y++)
	{
		for (int x = 0; x < SIZE; x++)
		{
			int i = correctTile.i;
			int j = correctTile.j;
			if (state->board[y][x] == goalState[i][j])
			{
				int localDist = abs(i - y) + abs(j - x);
				return localDist;
			}
		}
	}
	return 0;
}

int getManhattanDistance(puzzle *state)
{
	if (state->manhattanDistance != -1)
	{
		return state->manhattanDistance;
	}
	int dist = 0;		// variable that keeps the total manhattan distance
	Vector correctTile; // holds the index (i,j) of a tile in the given state
	for (int i = 0; i < SIZE; i++)
	{ // looping through all the tiles of the state
		for (int j = 0; j < SIZE; j++)
		{
			if (state->board[i][j] == goalState[i][j]) // skips if the tile is in the correct place
				continue;
			else
			{ /// Calculates the distance between 2 tiles (goalstate and misplaced tile)
				correctTile.setIndex(i, j);
				dist += distBetween2Tiles(state, correctTile); // serves as termination for nested loops if the distance has already been calculated
			}
		}
	}
	/// @note useful for getting the state with the lowest heuristic value and records the value
	state->manhattanDistance = dist + state->level;
	return dist + state->level;

}