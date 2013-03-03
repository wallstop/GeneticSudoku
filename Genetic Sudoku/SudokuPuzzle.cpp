#include "SudokuPuzzle.h"

SudokuPuzzle::SudokuPuzzle()
{

}

SudokuPuzzle::SudokuPuzzle(char *fileName)
{
	board = parseFile(fileName);
	staticBoard = board;
	sizeOfBoard = (int) sqrt(board.size());
	sizeOfMacroBlock = (int) sqrt(sizeOfBoard);

	initCells();
	//calculateSummation();
	evaluateFitness();
}

SudokuPuzzle::SudokuPuzzle(std::vector<int> existingStaticBoard)
{
	board = existingStaticBoard;
	staticBoard = board;
	sizeOfBoard = (int) sqrt(board.size());
	sizeOfMacroBlock = (int) sqrt(sizeOfBoard);

	initCells();
	//calculateSummation();
	evaluateFitness();
}

//Swap some cell with another inside a particular macroBlock of a previously-configured puzzle
SudokuPuzzle::SudokuPuzzle(const int block, const int origin, const int swap, SudokuPuzzle preConfig)
{
	int previousValue;

	int originRow;
	int originCol;
	int swapRow;
	int swapCol;

	sizeOfBoard = preConfig.getSizeOfBoard();
	sizeOfMacroBlock = preConfig.getSizeOfMacroBlock();
	board = preConfig.getBoard();
	staticBoard = * &(preConfig.getStaticBoard());

	originCol = (block % sizeOfMacroBlock) * sizeOfMacroBlock + (origin % sizeOfMacroBlock);
	originRow = (block / sizeOfMacroBlock) * sizeOfMacroBlock + (origin / sizeOfMacroBlock);
	swapCol = (block % sizeOfMacroBlock) * sizeOfMacroBlock + (swap % sizeOfMacroBlock);
	swapRow = (block / sizeOfMacroBlock) * sizeOfMacroBlock + (swap / sizeOfMacroBlock);

	assert(originCol < sizeOfBoard);
	assert(originRow < sizeOfBoard);
	assert(swapCol < sizeOfBoard);
	assert(swapRow < sizeOfBoard);
	//Only check if the cells can be swapped and their values are not equivalent
	if((staticBoard[convertCoordinates(originCol, originRow, sizeOfBoard)] == 0) && (staticBoard[convertCoordinates(swapCol, swapRow, sizeOfBoard)] == 0) && (board[convertCoordinates(originCol, originRow, sizeOfBoard)] != board[convertCoordinates(swapCol, swapRow, sizeOfBoard)]))
	{
		previousValue = board[convertCoordinates(originCol, originRow, sizeOfBoard)];
		board[convertCoordinates(originCol, originRow, sizeOfBoard)] = board[convertCoordinates(swapCol, swapRow, sizeOfBoard)];
		board[convertCoordinates(swapCol, swapRow, sizeOfBoard)] = previousValue;

		//calculateSummation();
		evaluateFitness();
	}
	else
	{
		fitness = preConfig.getFitness();
	}
}

void SudokuPuzzle::initCells()
{
	std::vector<int> usedNumbers;
	int currentNum;

	//Could be simplified to two loops from 0-sizeOfBoard, but then you'd have to mod&divide (even more) in order to get proper grid coordinates
	for(int i = 0; i < sizeOfMacroBlock; i++)
	{
		for(int j = 0; j < sizeOfMacroBlock; j++)
		{
			usedNumbers.clear();

			//Finds numbers naturally occuring in the Sudoku Puzzle
			for(int k = 0; k < sizeOfMacroBlock; k++)
				for(int l = 0; l < sizeOfMacroBlock; l++)
					if(staticBoard[convertCoordinates(j * sizeOfMacroBlock + l, i * sizeOfMacroBlock + k, sizeOfBoard)] > 0)
						usedNumbers.push_back(staticBoard[convertCoordinates(j * sizeOfMacroBlock + l, i * sizeOfMacroBlock + k, sizeOfBoard)]);

			//Fills in all the rest
			for(int k = 0; k < sizeOfMacroBlock; k++)
				for(int l = 0; l < sizeOfMacroBlock; l++)
					if(board[convertCoordinates(j * sizeOfMacroBlock + l, i * sizeOfMacroBlock + k, sizeOfBoard)] == 0)
					{
						do
						{
							currentNum = (rand() % sizeOfBoard) + 1;
						} while(std::find(usedNumbers.begin(), usedNumbers.end(), currentNum) != usedNumbers.end());

						board[convertCoordinates(j * sizeOfMacroBlock + l, i * sizeOfMacroBlock + k, sizeOfBoard)] = currentNum;
						usedNumbers.push_back(currentNum);
					}
		}
	}
}

void SudokuPuzzle::calculateSummation()
{
	targetSummation = 0;

	for(int i = 1; i <= sizeOfBoard; i++)
		targetSummation += i;
}

void SudokuPuzzle::evaluateFitness()
{
	
	std::vector<int> foundRowNumbers;
	std::vector<int> foundColNumbers;
	std::vector<int>::iterator itr;

	fitness = 0;

	for(int i = 0; i < sizeOfBoard; i++)
	{
		foundRowNumbers.clear();
		foundColNumbers.clear();

		for(int j = 0; j < sizeOfBoard; j++)
		{
			foundRowNumbers.push_back(board[convertCoordinates(j, i, sizeOfBoard)]);
			foundColNumbers.push_back(board[convertCoordinates(i, j, sizeOfBoard)]);
		}
	
		std::sort(foundRowNumbers.begin(), foundRowNumbers.end());
		std::sort(foundColNumbers.begin(), foundColNumbers.end());

		//Count duplicates
		for(itr = std::unique(foundRowNumbers.begin(), foundRowNumbers.end()); itr != foundRowNumbers.end(); itr++)
			fitness++;
		for(itr = std::unique(foundColNumbers.begin(), foundColNumbers.end()); itr != foundColNumbers.end(); itr++)
			fitness++;
	}
	
	/*
	int rowTotal;
	int colTotal;
	
	fitness = 0;

	for(int i = 0; i < sizeOfBoard; i++)
	{
		rowTotal = 0;
		colTotal = 0;

		for(int j = 0; j < sizeOfBoard; j++)
		{
			rowTotal += board[convertCoordinates(j, i, sizeOfBoard)];
			colTotal += board[convertCoordinates(i, j, sizeOfBoard)];
		}
		if(rowTotal > targetSummation)
			fitness += (rowTotal - targetSummation);
		if(rowTotal < targetSummation)
			fitness += (targetSummation - rowTotal);
		if(colTotal > targetSummation)
			fitness += (colTotal - targetSummation);
		if(colTotal < targetSummation)
			fitness += (targetSummation - colTotal);
	}
	*/
}

bool SudokuPuzzle::operator<(const SudokuPuzzle& other)
{
	return (fitness < other.fitness);
}

void SudokuPuzzle::printBoard()
{
	for(int i = 0; i < sizeOfBoard; i++)
	{
		for(int j = 0; j < sizeOfBoard; j++)
		{
			if(board[convertCoordinates(j, i, sizeOfBoard)] < 10)
				std::cout << " ";
			std::cout << board[convertCoordinates(j, i, sizeOfBoard)] << " ";
		}
		std::cout << std::endl;
	}
}

int SudokuPuzzle::getFitness()
{
	return fitness;
}

int SudokuPuzzle::getSizeOfBoard()
{
	return sizeOfBoard;
}

int SudokuPuzzle::getSizeOfMacroBlock()
{
	return sizeOfMacroBlock;
}

std::vector<int> SudokuPuzzle::getBoard()
{
	return board;
}

std::vector<int> SudokuPuzzle::getStaticBoard()
{
	return staticBoard;
}