#pragma once

#include <math.h>
#include <mutex>
#include <condition_variable>
#include "SudokuPuzzle.h"

#define populationSize 100
#define numberOfGenerations 50
#define minimumImprovement populationSize * .01
#define e 2.71828182845904523536

class GeneticPopulation
{
private:
	std::unique_lock<std::mutex> barrier;
	std::vector<SudokuPuzzle> genePool;
	std::vector<SudokuPuzzle> childPool;
	std::vector<int> staticBoard;
	double variance;
	bool optimalSolution;
	int sizeOfBoard;
	int sizeOfMacroBlock;

	void mutateSpecimen(SudokuPuzzle &, int);
	void checkOptimality();
	void sortGenePool();
	void spawnAdditionalMembers();
	void incrementVariance();
	void resetVariance();
	void spawnChildren();
	void improveGenePool();
	void advancePopulation();
public:
	GeneticPopulation(char *);
	GeneticPopulation(std::vector<SudokuPuzzle>, std::condition_variable, std::mutex);

	bool hasOptimal();
	void printGenePool();
};