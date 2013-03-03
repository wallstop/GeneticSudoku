#include "GeneticPopulation.h"

void initRandomSeed()
{
	srand((unsigned)time(0));
}

int main(int argc, char **argv)
{
	initRandomSeed();

	GeneticPopulation examplePopulation = GeneticPopulation("simpleSudoku.csv");

	system("PAUSE");

	return 0;
}