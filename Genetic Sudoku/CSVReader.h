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

std::vector<int> parseFile(char *fileName, char delim = ',');