#include "CSVReader.h"

std::vector<int> parseFile(char *fileName, char delim)
{
	int tempInt;
	std::ifstream file;
	std::vector<int> largeBuffer;
	std::string lineBuffer;
	std::regex pattern;

	pattern = ("[0-9]+");	//Any amount of contiguous numerical characters
	file = std::ifstream(fileName);

	if(!file)
	{
		std::cerr << "Could not open file." << std::endl;
		return std::vector<int>();
	}

	//There has to be a better way to do this...
	while(file)
	{
		std::getline(file, lineBuffer);
		const std::sregex_token_iterator end;
		//Iterate through while trying to pattern match
		for (std::sregex_token_iterator i(lineBuffer.cbegin(), lineBuffer.cend(), pattern); i != end; ++i)	
		{
			std::stringstream(*i) >> tempInt;
			largeBuffer.push_back(tempInt);
		}
	}

	return largeBuffer;
}