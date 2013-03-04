#include "GeneticPopulation.h"

//Creates a population out of a pre-existing number of SudokuPuzzle configurations
GeneticPopulation::GeneticPopulation(std::vector<SudokuPuzzle> initialConfig)
{
	//Called so vectors don't have to resize
	genePool.reserve(populationSize);
	childPool.reserve(populationSize);

	//Initialization
	genePool = initialConfig;
	staticBoard = genePool[0].getStaticBoard();
	optimalSolution = false;
	sizeOfBoard = genePool[0].getSizeOfBoard();
	sizeOfMacroBlock = genePool[0].getSizeOfMacroBlock();


	spawnAdditionalMembers();	//Fill up the genePool
	resetVariance();			//Initialize variance
	advancePopulation();		//Advance the population
}

//Fills up the genePool with randomly generated configurations
void GeneticPopulation::spawnAdditionalMembers()
{
	while(genePool.size() < populationSize)
		genePool.push_back(SudokuPuzzle(staticBoard));
}

//Wrapper method that spawns children, updates genePool, and checks for an optimal solution for n generations
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

	childPool.clear();	//Empties childPool

	//Creates populationSize number of children
	for(int i = 0; i < populationSize; i++)
	{
		preMateMutationRate = rand() % 100;	//Mutation of the genome pre-mating
		postMateMutationRate = rand() % 100;//Mutation of the genome post-mating
		parentConfiguration = rand() % 8;	//Determines which parents a child will have
		parentOrder = rand() % 2;			//Determines their order (A child will primarily be parent1)
		numMutations = 0;

		assert(genePool.size() == populationSize);

		//Parent config
		switch(parentConfiguration)
		{
		case 0:
			parent1 = genePool[0];	//Best 
			parent2 = genePool[(int) (rand() % (populationSize / 10))];	//Best 10%
			break;
		case 1:
			parent1 = genePool[0];
			parent2 = genePool[(rand() % (populationSize - 1)) + 1];	//Anyone but the best
			break;
		case 2:
			parent1 = genePool[(int) (rand() % (populationSize / 10))];
			parent2 = genePool[rand() % populationSize];	//Anyone
			break;
		case 3:
			parent1 = genePool[rand() % populationSize];
			parent2 = genePool[rand() % populationSize];
			break;
		case 4:
			parent1 = genePool[0];
			parent2 = SudokuPuzzle(staticBoard);	//Random config
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

			//Runs through a random range of macroBlocks
			for(int j = tempMin; j < tempMax; j++)
			{
				for(int k = 0; k < sizeOfBoard; k++)	//Tries to swap every pair
				{
					for(int l = (k + 1); l < sizeOfBoard; l++)
					{
						swapAnywayChance = rand() % 100;			//Random chance to perform the below swap anyway
						tempChild = SudokuPuzzle(j, k, l, child);

						if ((tempChild < child) || (!swapAnywayChance))	//Compares the two, only updates if the new configuration is better than the current one.
							child = tempChild;
					}
				}
			}
		}
		
		//Mutates the created child
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

		//Puts the child in the childPool
		childPool.push_back(child);
	}

	assert(childPool.size() == populationSize);
	//Orders the childPool
	std::sort(childPool.begin(), childPool.end());
}

//Swaps children with members of the genePool
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

	//Compares SudokuPuzzles at arbitrary positions within the two arrays
	for(int i = 0; i < numSwaps; i++)
	{
		index = rand() % populationSize;
		swapAnyways = rand() % 100;
		if((childPool[index] < genePool[index]) || (!swapAnyways))	//Updates the genePool 
		{
			genePool[index] = childPool[index];
			swapsMade++;
		}
	}

	//Determines if the minimum number of swaps were made, increases chance for pre-mate mutation rate if the minimum wasn't met
	if(swapsMade < minimumImprovement)
		incrementVariance();
	else
		resetVariance();

	sortGenePool();
}

//I'm not sure if this method works properly
//	Should randomly swap around cells within a macro block some number of times
//	This can be highly optimized with a member function, such that evaluateFitness() is only called one time (after all the swaps)
//		and no new objects are created. @TODO
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
	//Shuffle everything but the best for a good representation of the population (avoids convergance of local maxima)
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

//Should represent a logarithmic curve
void GeneticPopulation::incrementVariance()
{
	variance = pow(e, variance);
	variance += 30;
	variance = log(variance);
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