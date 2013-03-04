#pragma once

#include <stdio.h>
#include <vector>
#include "CSVReader.h"
#include "Utils.h"

class SudokuPuzzle
{
private:
	int sizeOfMacroBlock;			//length of sides of each "macroBlock", or cell of a Sudoku Puzzle
	int sizeOfBoard;				//the n of an n x n Sudoku Puzzle
	std::vector<int> board;			//Representation of the current configuration of the Sudoku board
	std::vector<int> staticBoard;	//Representation of the initial configuration of the Sudoku board
	int fitness;					//How many total row and column-wise conflicts the configuration has

	void initCells();				//Used to set up an initial configuration after reading in a file
	void evaluateFitness();			//Evaluates the configuration's number of conflicts
public:
	SudokuPuzzle();					//Empty constructor so that empty objects can be created
	SudokuPuzzle(char *);			//Creates a board from some valid .csv representation of a Sudoku file
	SudokuPuzzle(std::vector<int>);				//Creates a board from an initial Sudoku configuration
	SudokuPuzzle(const int, const int, const int, SudokuPuzzle);	//Creates an object by swapping around two cells within a macroBlock of an existing Sudoku Puzzle
	
	bool operator<(const SudokuPuzzle&);	//Used for sorting comparisons
	//Gets for member variables
	int getSizeOfMacroBlock();
	int getSizeOfBoard();
	int getFitness();
	std::vector<int> getBoard();
	std::vector<int> getStaticBoard();
	void printBoard();
};