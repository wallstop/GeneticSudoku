#include "PopulationCongregator.h"

//Starts a PopulationCoordinator based off of a SudokuPuzzle file
void init(char *file)
{
	optimalSolution = false;
	fileName = file;
	threadConfigs.reserve(numThreads);
	
	//Sets up initial SudokuPuzzles used for population seeding
	for(int i = 0; i < numThreads; i++)
		threadConfigs.push_back(std::vector<SudokuPuzzle>());

	for(int i = 0; i < populationSize; i++)
		threadConfigs[i % numThreads].push_back(SudokuPuzzle(fileName));

	assert(threadConfigs.size() == numThreads);
	spawnThreads();
}

//Controls a specific GeneticPopulation
void workerThread(int threadID)
{
	//Run forever!
	while(true)
	{
		//Create a new genetic population every generation (in reference to the PopulationCoordinator) based off of pre-existing SudokuPuzzles
		GeneticPopulation smallPopulace(threadConfigs[threadID]);
		std::vector<SudokuPuzzle> tempArray;
		tempArray = smallPopulace.getPopulationSegment();

		assert(tempArray.size() == (populationSize /2));

		//Critical region:
		//	unique_lock is used to simulate a barrier
		//	This code locks threadMutex
		std::unique_lock<std::mutex> lock(threadMutex);

		//If the solution is found, update global variables
		if(tempArray[0].getFitness() == 0)
		{
			optimalSolution = true;
			theSolution = tempArray[0];
		}

		//Update the global array of Specimens
		for(int i = 0; i < tempArray.size(); i++)
			overLordArray.push_back(tempArray[i]);

		//Let the master thread know we're finished
		finishedThreads++;

		condVar.wait(lock);	//Chill out here dog
	}
}

//Empties the pools of SudokuPuzzles that will be sent to threads
void clearThreadConfigs()
{
	threadConfigs.clear();
	for(int i = 0; i < numThreads; i++)
		threadConfigs.push_back(std::vector<SudokuPuzzle>());
}

void spawnThreads()
{
	finishedThreads = 0;

	for(int i = 0; i < numThreads; i++)
		threadPool.push_back(std::thread(workerThread, i));
}

//Master thread:
void run()
{
	int generationCounter;
	std::chrono::high_resolution_clock::time_point time;

	time = std::chrono::high_resolution_clock::now();
	generationCounter = 0;

	while(true)
	{
		//Since we're only reading, we don't care about locking finishedThreads
		while(finishedThreads != numThreads)
			std::this_thread::sleep_for(std::chrono::milliseconds(2000));	//Wake up every two seconds to check if the workers are finished
	
		//Only reaches here when all worker threads are waiting at the barrier

		//Information print outs, can be abstracted to methods...
		std::cout << "Generation: " << ++generationCounter << std::endl;
		clearThreadConfigs();

		//Evenly distribute the gathered SudokuPuzzles among the threads, this mixes the populations
		for(int i = 0; i < overLordArray.size(); i++)
			threadConfigs[i % numThreads].push_back(overLordArray[i]);

		//Check to see if an optimal solution has been found (this is redundant)
		std::sort(overLordArray.begin(), overLordArray.end());
		std::cout << "Current best fitness: " << overLordArray[0].getFitness() << std::endl;
		
		//Print the best board every ten generations
		if(generationCounter % 10 == 0)
			overLordArray[0].printBoard();

		overLordArray.clear();
		finishedThreads = 0;

		//If no optimal solution has been found, continue to populate/breed
		if(!optimalSolution)
			condVar.notify_all();
		else
			break;
	}

	//Final config outputs
	std::chrono::high_resolution_clock::time_point time2 = std::chrono::high_resolution_clock::now();
	std::cout << "Operation took: " << std::chrono::duration_cast<std::chrono::milliseconds>(time2 - time).count() << std::endl;
}

//Seeds random
void initRandomSeed()
{
	srand((unsigned)time(0));
}

int main(int argc, char **argv)
{
	initRandomSeed();

	//Checks for command-line parameters, tries to execute with "sudoku1.csv" if none are found
	if(argc < 2)
		init("sudoku1.csv");
	else
		init(argv[1]);

	std::cout<< "Starting threads... " << std::endl;
	run();

	theSolution.printBoard();
	//Just in case this wasn't run from the console, should be taken out later
	system("PAUSE");

	return 0;
}