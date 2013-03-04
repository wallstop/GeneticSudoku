#pragma once

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>
#include <regex>
#include <assert.h>

//Returns a vector of ints that represents a Sudoku Puzzle, 0 represents empty space
//	Only parses properly formatted (CRLF-line ended, comma-delineated text files) file representation of Sudoku
std::vector<int> parseFile(char *fileName, char delim = ',');