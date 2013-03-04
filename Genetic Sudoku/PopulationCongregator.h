/*	@Description: Creates an "overseer" of a number of threads that create genetic populations. Every x generations,
 *		each worker thread returns a set of Sudoku Puzzles, which the overseer than shuffles around and re-distributes.
 *		This process continues until an optimal solution to a Sudoku Puzzle has been found.
 */


#pragma once

#include <thread>
#include "GeneticPopulation.h"

#define numThreads 4

//Making this class-based makes it difficult to run threads, there'd have to be another layer of class-based abstraction which is gross and unecessary
//	So instead, these are method prototypes for a "driver"

bool optimalSolution;	//Whether or not a solved Sudoku Puzzle configuration has been found
int finishedThreads;	//Used in simulation of a barrier
char *fileName;			//File used for initial Sudoku Puzzle configuration
std::vector<SudokuPuzzle> overLordArray;	//Used to congregate populations from worker threads
std::vector<std::vector<SudokuPuzzle>> threadConfigs;	//Used to distribute Sudoku Puzzles to threads
std::vector<std::thread> threadPool;		
std::mutex threadMutex;				//Used in simulation of a barrier
std::condition_variable condVar;	//Used in simulation of a barrier
SudokuPuzzle theSolution;			//The solved Sudoku Puzzle for the given configuration

void clearThreadConfigs();			//Clears all collected specimens after each generation
void workerThread(int);				//Runs an instance of GeneticPopulation, simulates a "colony" of Sudoku Puzzles
void init(char *);					//Necessary initializations
void spawnThreads();				//Initializes the threadPool
void run();							//Runs the PopulationCongregator (threadPools, colonies, and all)
