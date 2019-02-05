// PPQCacheExtractor.cpp : Ten plik zawiera funkcję „main”. W nim rozpoczyna się i kończy wykonywanie programu.
//

#include "pch.h"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iterator>
#include <direct.h>

char* FileIntoByteVector(char* filename)
{
	std::ifstream ifs(filename, std::ios::binary);

	if (!ifs.is_open())
	{
		std::cout << "Error opening file " << filename << std::endl;
		return NULL;
	}

	ifs.seekg(0, ifs.end);
	int length = ifs.tellg();
	char* result = new char[length];
	
	ifs.seekg(0, ifs.beg);
	ifs.read(result, length);

	return result;
}

int main(int argc, char *argv[])
{
	//int cacheFileCount;
	int fileCount;
	char* cacheFileBytes;
	//std::vector<char> cacheFileBytes;
	std::vector<int> fileSize;
	std::vector<std::string> fileName;
	std::vector<unsigned char> curFileBytes;

	if (argc < 2)
	{
		std::cout << "Give at least one file to extract from.\n";
		return 0;
	}

	//cacheFileCount = 

	// Copy bytes from cache file into cacheFileBytes
	cacheFileBytes = FileIntoByteVector(argv[1]);

	if (cacheFileBytes == NULL)
		return 1;

	// Get first 2 bytes and store them in fileCount
	fileCount = (int)((unsigned char)cacheFileBytes[0] | (unsigned char)cacheFileBytes[1] << 8);

	// Starting from byte 4 fill the fileSize vector with file sizes read as 2 bytes divided by 2 empty bytes fileCount times.
	for (int i = 0; i < fileCount; i++)
		fileSize.push_back((int)((unsigned char)cacheFileBytes[4 + i * 4] | (unsigned char)cacheFileBytes[4 + i * 4 + 1] << 8));
	

	// There are 8 bytes for each file that I don't know the purpose for yet. We skip them since they're not needed for extraction
	int pos = 4 + 4 * fileCount + 8 * fileCount;
	// TODO: Find the purpose of these 8 bytes

	std::string curFileName;
	//char* path;
	// Find the file names and put them in fileName vector
	for (int i = 0; i < fileCount; i++)
	{
		// Reset the file name string
		curFileName.clear();

		// Find the next non 00 byte
		while (cacheFileBytes[pos] == 0)
			pos++;

		// Copy every char into the string until you encounter 00
		while (cacheFileBytes[pos] != 0)
		{
			// TODO: If we encounter a directory, create it
			if (cacheFileBytes[pos] == '/')
				curFileName.clear();
			else
				curFileName += cacheFileBytes[pos];

			pos++;
		}

		// Insert the filename into fileName vector
		curFileName += '\0';
		fileName.push_back(curFileName);
	}

	std::ofstream out_file;

	for (int i = 0; i < fileCount; i++)
	{
		curFileBytes.clear();

		// Find the next non 00 byte
		while (cacheFileBytes[pos] == 0)
			pos++;

		// Copy fileSize bytes into curFileBytes.
		for (int j = 0; j < fileSize[i]; j++)
		{
			curFileBytes.push_back((unsigned char)cacheFileBytes[pos]);
			pos++;
		}

		// Create new file with name fileName that consists of every copied byte stored in curFileBytes. Save the file.
		out_file.open(fileName[i], std::ios::out | std::ios::binary);
		for (int j = 0; j < fileSize[i]; j++)
			out_file << curFileBytes[j];

		out_file.close();
		//std::ofstream(fileName[i], std::ios::out | std::ios::binary).write((char*)&curFileBytes, fileSize[i] * sizeof(char));
	}
	

	return 0;
}
