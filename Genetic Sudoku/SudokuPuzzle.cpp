#include "SudokuPuzzle.h"

//This lets you call SudokuPuzzle x;
SudokuPuzzle::SudokuPuzzle()
{

}

//Parses a Sudoku file
SudokuPuzzle::SudokuPuzzle(char *fileName)
{
	board = parseFile(fileName);
	staticBoard = board;
	sizeOfBoard = (int) sqrt(board.size());
	sizeOfMacroBlock = (int) sqrt(sizeOfBoard);

	//Fills in empty cells and determines conflicts
	initCells();
	evaluateFitness();
}

//Fills in a SudokuPuzzle based on a pre-existing initial board
SudokuPuzzle::SudokuPuzzle(std::vector<int> existingStaticBoard)
{
	board = existingStaticBoard;
	staticBoard = board;
	sizeOfBoard = (int) sqrt(board.size());
	sizeOfMacroBlock = (int) sqrt(sizeOfBoard);

	//Fills in empty cells and determines conflicts
	initCells();
	evaluateFitness();
}

//Swap some cell with another inside a particular macroBlock of a previously-configured puzzle
//	0 <= origin & swap < sizeOfBoard
//	These values correspond to the cell position within the macroBlock, 0 for top-left, sizeOfBoard-1 for bottom right
SudokuPuzzle::SudokuPuzzle(const int block, const int origin, const int swap, SudokuPuzzle preConfig)
{
	int previousValue;

	int originRow;
	int originCol;
	int swapRow;
	int swapCol;

	//Properly initialize
	sizeOfBoard = preConfig.getSizeOfBoard();
	sizeOfMacroBlock = preConfig.getSizeOfMacroBlock();
	board = preConfig.getBoard();
	staticBoard = * &(preConfig.getStaticBoard());

	//Math wizardry to determine what position in the single-dimension array the cells match up to
	originCol = (block % sizeOfMacroBlock) * sizeOfMacroBlock + (origin % sizeOfMacroBlock);
	originRow = (block / sizeOfMacroBlock) * sizeOfMacroBlock + (origin / sizeOfMacroBlock);
	swapCol = (block % sizeOfMacroBlock) * sizeOfMacroBlock + (swap % sizeOfMacroBlock);
	swapRow = (block / sizeOfMacroBlock) * sizeOfMacroBlock + (swap / sizeOfMacroBlock);

	//Make sure everything's ok
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

		evaluateFitness();
	}
	else	//Otherwise the board configuration is the same, and the fitness can simply be copied instead of evaluated
	{
		fitness = preConfig.getFitness();
	}
}

//Populates the board with values such that there are no conflicts within a macroBlock
void SudokuPuzzle::initCells()
{
	std::vector<int> usedNumbers;
	int currentNum;

	//Could be simplified to two loops from 0-sizeOfBoard, but then you'd have to mod&divide (even more) in order to get proper grid coordinates
	for(int i = 0; i < sizeOfMacroBlock; i++)
	{
		for(int j = 0; j < sizeOfMacroBlock; j++)
		{
			usedNumbers.clear();	//Reset number count

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

//Determines the number of conflicts in the current configuration
void SudokuPuzzle::evaluateFitness()
{
	
	std::vector<int> foundRowNumbers;	//Number of conflicts in the current row
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
	
		//Sorts the vectors so that the numbers are in order. This is used to later call std::unique to count duplicates 
		std::sort(foundRowNumbers.begin(), foundRowNumbers.end());
		std::sort(foundColNumbers.begin(), foundColNumbers.end());

		//Count duplicates
		for(itr = std::unique(foundRowNumbers.begin(), foundRowNumbers.end()); itr != foundRowNumbers.end(); itr++)
			fitness++;
		for(itr = std::unique(foundColNumbers.begin(), foundColNumbers.end()); itr != foundColNumbers.end(); itr++)
			fitness++;
	}
	
}

//Allows SudokuPuzzles to be compared to each other based off of fitness
bool SudokuPuzzle::operator<(const SudokuPuzzle& other)
{
	return (fitness < other.fitness);
}

void SudokuPuzzle::randomize(int numMutations)
{
	int previousValue;
	int block;
	int origin;
	int swap;
	int originCol;
	int originRow;
	int swapCol;
	int swapRow;

	for(int i = 0; i < numMutations; i++)
	{
		block = rand() % sizeOfBoard;
		origin = rand() % sizeOfBoard;
		swap = rand() % sizeOfBoard;

		//Math wizardry to determine what position in the single-dimension array the cells match up to
		originCol = (block % sizeOfMacroBlock) * sizeOfMacroBlock + (origin % sizeOfMacroBlock);
		originRow = (block / sizeOfMacroBlock) * sizeOfMacroBlock + (origin / sizeOfMacroBlock);
		swapCol = (block % sizeOfMacroBlock) * sizeOfMacroBlock + (swap % sizeOfMacroBlock);
		swapRow = (block / sizeOfMacroBlock) * sizeOfMacroBlock + (swap / sizeOfMacroBlock);

		//Make sure everything's ok
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
		}
	}

	evaluateFitness();
}

//Prints the board configuration
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

void SudokuPuzzle::replaceCell(int block, int origin, int swap)
{
	int originCol;
	int originRow;
	int swapRow;
	int swapCol;
	int previousValue;

	originCol = (block % sizeOfMacroBlock) * sizeOfMacroBlock + (origin % sizeOfMacroBlock);
	originRow = (block / sizeOfMacroBlock) * sizeOfMacroBlock + (origin / sizeOfMacroBlock);

	for(int i = 0; i < sizeOfBoard; i++)
	{
		if(board[convertCoordinates((block % sizeOfMacroBlock) * sizeOfMacroBlock + i % sizeOfMacroBlock, (block / sizeOfMacroBlock) * sizeOfMacroBlock + i / sizeOfMacroBlock, sizeOfBoard)] == swap)
		{
			swapCol = (block % sizeOfMacroBlock) * sizeOfMacroBlock + (i % sizeOfMacroBlock);
			swapRow = (block / sizeOfMacroBlock) * sizeOfMacroBlock + (i / sizeOfMacroBlock);
			break;
		}

	}

	//Todo: 
	if((staticBoard[convertCoordinates(originCol, originRow, sizeOfBoard)] == 0) && (staticBoard[convertCoordinates(swapCol, swapRow, sizeOfBoard)] == 0) && (board[convertCoordinates(originCol, originRow, sizeOfBoard)] != board[convertCoordinates(swapCol, swapRow, sizeOfBoard)]))
	{
		previousValue = board[convertCoordinates(originCol, originRow, sizeOfBoard)];
		board[convertCoordinates(originCol, originRow, sizeOfBoard)] = board[convertCoordinates(swapCol, swapRow, sizeOfBoard)];
		board[convertCoordinates(swapCol, swapRow, sizeOfBoard)] = previousValue;
		evaluateFitness();
	}
}

int SudokuPuzzle::getCellAt(int block, int x)
{
	int originCol;
	int originRow;

	originCol = (block % sizeOfMacroBlock) * sizeOfMacroBlock + (x % sizeOfMacroBlock);
	originRow = (block / sizeOfMacroBlock) * sizeOfMacroBlock + (x / sizeOfMacroBlock);
	return board[convertCoordinates(originCol, originRow, sizeOfBoard)];
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