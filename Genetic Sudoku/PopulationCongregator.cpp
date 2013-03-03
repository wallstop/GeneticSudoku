#include "PopulationCongregator.h"

void init(char *file)
{
	optimalSolution = false;
	fileName = file;
	threadConfigs.reserve(numThreads);
	
	for(int i = 0; i < numThreads; i++)
		threadConfigs.push_back(std::vector<SudokuPuzzle>());

	for(int i = 0; i < populationSize; i++)
		threadConfigs[i % numThreads].push_back(SudokuPuzzle(fileName));

	assert(threadConfigs.size() == numThreads);
	spawnThreads();
}

void workerThread(int threadID)
{
	while(true)
	{
		GeneticPopulation smallPopulace(threadConfigs[threadID] /*, std::ref(condVar), std::ref(threadMutex) */);
		std::vector<SudokuPuzzle> tempArray;
		tempArray = smallPopulace.getPopulationSegment();

		assert(tempArray.size() == (populationSize /2));

		std::unique_lock<std::mutex> lock(threadMutex);
		if(tempArray[0].getFitness() == 0)
		{
			//lock.lock();
			optimalSolution = true;
			theSolution = tempArray[0];
			//lock.unlock();
		}

		//Simulates barrier
		//lock.lock();
		//threadMutex.lock();
		for(int i = 0; i < tempArray.size(); i++)
			overLordArray.push_back(tempArray[i]);
		finishedThreads++;
		//threadMutex.unlock();
		//lock.unlock();
		condVar.wait(lock);	//Chill out here dog
	}
}

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

void run()
{
	int generationCounter;

	generationCounter = 0;
	while(true)
	{
		//Since we're reading, we don't care about locking finishedThreads
		while(finishedThreads != numThreads)
			std::this_thread::sleep_for(std::chrono::milliseconds(2000));
	
		//Barrier has been hit here!
		std::cout << "Generation: " << ++generationCounter << std::endl;
		clearThreadConfigs();
		for(int i = 0; i < overLordArray.size(); i++)
			threadConfigs[i % numThreads].push_back(overLordArray[i]);

		overLordArray.clear();
		threadMutex.lock();
		finishedThreads = 0;
		threadMutex.unlock();

		if(!optimalSolution)
			condVar.notify_all();
		else
			break;
	}
}

void initRandomSeed()
{
	srand((unsigned)time(0));
}

int main(int argc, char **argv)
{
	initRandomSeed();

	//GeneticPopulation examplePopulation = GeneticPopulation("sudoku1.csv");
	if(argc < 2)
		init("sudoku1.csv");
	else
		init(argv[1]);

	std::cout<< "Starting threads... " << std::endl;
	run();

	theSolution.printBoard();
	system("PAUSE");

	return 0;
}