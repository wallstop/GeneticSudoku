#pragma once

#include <stdio.h>
#include <vector>
#include "CSVReader.h"
#include "Utils.h"

class SudokuPuzzle
{
private:
	int sizeOfMacroBlock;
	int sizeOfBoard;
	std::vector<int> board;
	std::vector<int> staticBoard;
	int fitness;
	int targetSummation;

	void calculateSummation();
	void initCells();
	void evaluateFitness();
public:
	SudokuPuzzle();
	SudokuPuzzle(char *);
	SudokuPuzzle(std::vector<int>);
	SudokuPuzzle(const int, const int, const int, SudokuPuzzle);
	
	bool operator<(const SudokuPuzzle&);	//Used for sorting comparisons
	int getSizeOfMacroBlock();
	int getSizeOfBoard();
	int getFitness();
	std::vector<int> getBoard();
	std::vector<int> getStaticBoard();
	void printBoard();
};