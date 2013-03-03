#pragma once

#include <thread>
#include "GeneticPopulation.h"

#define numThreads 4

//Making this class-based makes it difficult to run threads, there'd have to be another layer of class-based abstraction which is gross and unecessary

bool optimalSolution;
int finishedThreads;
char *fileName;
std::vector<SudokuPuzzle> overLordArray;
std::vector<std::vector<SudokuPuzzle>> threadConfigs;
std::vector<std::thread> threadPool;
std::mutex threadMutex;
std::condition_variable condVar;
SudokuPuzzle theSolution;

void clearThreadConfigs();
void workerThread(int);
void init(char *);
void spawnThreads();
void managePopulations();
void run();
