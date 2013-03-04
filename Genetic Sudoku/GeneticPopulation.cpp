#include "GeneticPopulation.h"

//Test constructor to make sure sorting and such works fine
GeneticPopulation::GeneticPopulation(char *fileName)
{
	genePool.reserve(populationSize);
	childPool.reserve(populationSize);
	genePool.push_back(SudokuPuzzle(fileName));
	staticBoard = genePool[0].getStaticBoard();
	optimalSolution = false;
	sizeOfBoard = genePool[0].getSizeOfBoard();
	sizeOfMacroBlock = genePool[0].getSizeOfMacroBlock();

	printGenePool();
	spawnAdditionalMembers();

	assert(genePool.size() == populationSize);
	sortGenePool();
	resetVariance();
	advancePopulation();
	printGenePool();
	genePool[0].printBoard();
}

GeneticPopulation::GeneticPopulation(std::vector<SudokuPuzzle> initialConfig /*, std::condition_variable &condVar, std::mutex &threadMutex */)
{
	//barrier = std::unique_lock<std::mutex>(threadMutex);
	genePool.reserve(populationSize);
	childPool.reserve(populationSize);
	genePool = initialConfig;
	staticBoard = genePool[0].getStaticBoard();
	optimalSolution = false;
	sizeOfBoard = genePool[0].getSizeOfBoard();
	sizeOfMacroBlock = genePool[0].getSizeOfMacroBlock();

	spawnAdditionalMembers();
	resetVariance();
	advancePopulation();
	//condVar.wait(barrier);
}

//Fills up the genePool with randomly generated configurations
void GeneticPopulation::spawnAdditionalMembers()
{
	while(genePool.size() < populationSize)
		genePool.push_back(SudokuPuzzle(staticBoard));
}

void GeneticPopulation::advancePopulation()
{
	for(int i = 0; (i < numberOfGenerations) && (!optimalSolution); i++)
	{
		spawnChildren();
		improveGenePool();
		checkOptimality();
	}
}

void GeneticPopulation::spawnChildren()
{
	int preMateMutationRate;
	int postMateMutationRate;
	int parentConfiguration;
	bool parentOrder;
	int numMutations;
	int swapAnywayChance;
	//These could be pointers
	SudokuPuzzle parent1;
	SudokuPuzzle parent2;
	SudokuPuzzle child;
	SudokuPuzzle tempChild;

	childPool.clear();

	for(int i = 0; i < populationSize; i++)
	{
		preMateMutationRate = rand() % 100;
		postMateMutationRate = rand() % 100;
		parentConfiguration = rand() % 8;
		parentOrder = rand() % 2;
		numMutations = 0;

		//Select different mates
		assert(genePool.size() == populationSize);
		switch(parentConfiguration)
		{
		case 0:
			parent1 = genePool[0];
			parent2 = genePool[(int) (rand() % (populationSize / 10))];
			break;
		case 1:
			parent1 = genePool[0];
			parent2 = genePool[(rand() % (populationSize - 1)) + 1];
			break;
		case 2:
			parent1 = genePool[(int) (rand() % (populationSize / 10))];
			parent2 = genePool[rand() % populationSize];
			break;
		case 3:
			parent1 = genePool[rand() % populationSize];
			parent2 = genePool[rand() % populationSize];
			break;
		case 4:
			parent1 = genePool[0];
			parent2 = SudokuPuzzle(staticBoard);
			break;
		case 5:
			parent1 = genePool[rand() % populationSize];
			parent2 = SudokuPuzzle(staticBoard);
			break;
		case 6:
			parent1 = genePool[(int) (rand() % (populationSize / 10))];
			parent2 = SudokuPuzzle(staticBoard);
			break;
		case 7:
			parent1 = SudokuPuzzle(staticBoard);
			parent2 = SudokuPuzzle(staticBoard);
			break;
		}

		//Swap the parents around
		if(parentOrder)
		{
			SudokuPuzzle tempSpecimen;
	
			tempSpecimen = parent1;
			parent1 = parent2;
			parent2 = tempSpecimen;
		}

		child = parent1;

		if(preMateMutationRate < variance)
			mutateSpecimen(std::ref(child), preMateMutationRate);
	
		//Optimize, has to be better way
		//Fit a random number of macroBlocks

		//for(int j = 0; j < sizeOfBoard; j++)
		{
			int tempMax;
			int tempMin;

			tempMax = rand() % sizeOfBoard;
			tempMin = rand() % sizeOfBoard;
			
			if(tempMin > tempMax)
			{
				int temp = tempMin;
				tempMin = tempMax;
				tempMax = temp;
			}
			tempMax++;

			for(int j = tempMin; j < tempMax; j++)
			{
				for(int k = 0; k < sizeOfBoard; k++)
				{
					for(int l = (k + 1); l < sizeOfBoard; l++)
					{
						swapAnywayChance = rand() % 100;
						tempChild = SudokuPuzzle(j, k, l, child);
						if ((tempChild < child) || (!swapAnywayChance))
							child = tempChild;
					}
				}
			}
		}
		
		
		switch(postMateMutationRate)
		{
		case 0:
			numMutations += 2 * sizeOfBoard;
		case 1:
			numMutations += sizeOfBoard;
		case 2:
			numMutations++;
		case 3:
			numMutations++;
		case 4:
			numMutations++;
		case 5:
			numMutations++;
		case 6:
			numMutations++;
		case 7:
			numMutations++;
		case 8:
			numMutations++;
			mutateSpecimen(std::ref(child), numMutations);
			break;
		}

		childPool.push_back(child);
	}

	assert(childPool.size() == populationSize);
	std::sort(childPool.begin(), childPool.end());
}

void GeneticPopulation::improveGenePool()
{
	int numSwaps;
	int swapsMade;
	int index;
	int swapAnyways;

	swapsMade = 0;
	numSwaps = (int) (rand() % 20) + minimumImprovement;

	//Random chance to compare the best two
	if(numSwaps % 2 == 0)
		if(childPool[0] < genePool[0])
		{
			genePool[0] = childPool[0];
			swapsMade++;
		}

	for(int i = 0; i < numSwaps; i++)
	{
		index = rand() % populationSize;
		swapAnyways = rand() % 100;
		if((childPool[index] < genePool[index]) || (!swapAnyways))
		{
			genePool[index] = childPool[index];
			swapsMade++;
		}
	}

	if(swapsMade < minimumImprovement)
		incrementVariance();
	else
		resetVariance();

	sortGenePool();
}

//I'm not sure if this method works properly
void GeneticPopulation::mutateSpecimen(SudokuPuzzle &config, int mutationAmount)
{
	for(int i = 0; i < mutationAmount; i++)
	{
		config = SudokuPuzzle(rand() % sizeOfBoard, rand() % sizeOfBoard, rand() % sizeOfBoard, config);
	}
}

//Returns the best 50% of the genePool
std::vector<SudokuPuzzle> GeneticPopulation::getPopulationSegment()
{
	//Shuffle everything but the best 
	std::random_shuffle(++genePool.begin(), genePool.end());
	genePool.resize(populationSize / 2);
	return genePool;
}

//Sorting the genePool allows us to find the best and worst members simply be indexing into the genePool
void GeneticPopulation::sortGenePool()
{
	std::sort(genePool.begin(), genePool.end());
}

void GeneticPopulation::resetVariance()
{
	variance = log(1);
}

void GeneticPopulation::incrementVariance()
{
	variance = pow(e, variance);
	variance += 30;
	variance = log(variance);
}

void GeneticPopulation::printGenePool()
{
	std::cout << genePool[0].getFitness() << std::endl;
}

void GeneticPopulation::checkOptimality()
{
	if(genePool[0].getFitness() == 0)
		optimalSolution = true;
}
bool GeneticPopulation::hasOptimal()
{
	return optimalSolution;
}