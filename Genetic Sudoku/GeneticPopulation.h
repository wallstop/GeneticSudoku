#pragma once

#include <math.h>
#include <mutex>
#include <condition_variable>
#include "SudokuPuzzle.h"

//Make sure populationSize is an even number, threads return exactly half of this. These numbers can be tweaked
#define populationSize 100	
#define numberOfGenerations 100
//minimumImprovement defines the minimum number of replacements per generation that need to happen, otherwise increase the randomness of pre-mate genome mutation
#define minimumImprovement 25

//Probably could have used Math.e instead
#define e 2.71828182845904523536

class GeneticPopulation
{
private:
	std::vector<SudokuPuzzle> genePool;			//The current gene pool
	std::vector<SudokuPuzzle> childPool;		//The collection of children that are spawned every generation
	std::vector<int> staticBoard;				//The initial board which should be shared across all members of the population
	double variance;							//Used to test the randomness of pre-mate genome mutation
	bool optimalSolution;
	int sizeOfBoard;
	int sizeOfMacroBlock;						

	void mutateSpecimen(SudokuPuzzle &, int);	//Swaps around some number of cells in some SudokuPuzzle configuration
	void checkOptimality();						//Determines if an optimal solution has been found
	void sortGenePool();						//Sorts the genePool so that the most-fit members is at index 0, and the least-fit is at the end
	void spawnAdditionalMembers();				//When a GeneticPopulation is given an initial array(via the constructor), create additional members until populationSize has been reached
	void incrementVariance();					//If not enough swaps have been made, increase the randomness of pre-mate genome mutation
	void resetVariance();						//If enough swaps have been made, reset the randomness of pre-mate genome mutation
	void spawnChildren();						//Mates parents from genePool and populates the childPool
	void improveGenePool();						//Swaps children with members of the current gene pool
	void advancePopulation();					//Caller function to advance the current function for n generations
public:
	GeneticPopulation(std::vector<SudokuPuzzle>);

	std::vector<SudokuPuzzle> getPopulationSegment();	//Returns a subset of the generated population to the PopulationCongregator
	bool hasOptimal();									//Whether or not an optimal solution has been found
};