// PPQCacheExtractor.cpp : Ten plik zawiera funkcję „main”. W nim rozpoczyna się i kończy wykonywanie programu.
//

#include "pch.h"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iterator>
#include <Windows.h>

// Opens file in binary mode and extracts bytes into char array
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

// Converting string so it can be used in CreateDirectory
std::wstring s2ws(std::string s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}

int extract(char cacheName[])
{
	char* cacheFileBytes;

	unsigned int fileCount;
	std::vector<unsigned int> fileSize;
	std::vector<unsigned int> fileNameStart;
	std::vector<unsigned int> fileContentStart;
	std::vector<std::string> fileName;
	std::vector<unsigned char> curFileBytes;

	std::string baseDir(cacheName);

	baseDir += "_out/";

	// Create the output directory. If it fails not because it already exists, error, end
	// Platform dependent rn (Windows).
	if (!CreateDirectory(s2ws(baseDir).c_str(), NULL) && ERROR_ALREADY_EXISTS != GetLastError())
	{
		std::cout << "Error creating directory " << baseDir << "\n";
		return 1;
	}

	// Copy bytes from cache file into cacheFileBytes
	cacheFileBytes = FileIntoByteVector(cacheName);

	if (cacheFileBytes == NULL)
		return 1;

	// Get first 4 bytes and store them in fileCount
	fileCount = (unsigned int)((unsigned char)cacheFileBytes[0] | (unsigned char)cacheFileBytes[1] << 8 | (unsigned char)cacheFileBytes[1] << 16 | (unsigned char)cacheFileBytes[1] << 24);

	// Starting from byte 4 fill the fileSize vector with file sizes read as 4 bytes fileCount times.
	int pos = 4;
	for (int i = 0; i < fileCount; i++)
	{
		fileSize.push_back((unsigned int)((unsigned char)cacheFileBytes[pos + i * 4] | (unsigned char)cacheFileBytes[pos + i * 4 + 1] << 8
			| (unsigned char)cacheFileBytes[pos + i * 4 + 2] << 16 | (unsigned char)cacheFileBytes[pos + i * 4 + 3] << 24));
	}
	
	// Read fileCount vaules and assign them to fileNameStart
	pos += fileCount * 4;
	for (int i = 0; i < fileCount; i++)
	{
		fileNameStart.push_back((unsigned int)((unsigned char)cacheFileBytes[pos + i * 4] | (unsigned char)cacheFileBytes[pos + i * 4 + 1] << 8
			| (unsigned char)cacheFileBytes[pos + i * 4 + 2] << 16 | (unsigned char)cacheFileBytes[pos + i * 4 + 3] << 24));
	}

	// Read fileCount vaules and assign them to fileContentStart
	pos += fileCount * 4;
	for (int i = 0; i < fileCount; i++)
	{
		fileContentStart.push_back((unsigned int)((unsigned char)cacheFileBytes[pos + i * 4] | (unsigned char)cacheFileBytes[pos + i * 4 + 1] << 8
			| (unsigned char)cacheFileBytes[pos + i * 4 + 2] << 16 | (unsigned char)cacheFileBytes[pos + i * 4 + 3] << 24));
	}

	std::string curFileName;
	std::string path;
	// Find the file names and put them in fileName vector
	for (int i = 0; i < fileCount; i++)
	{
		// Reset the file name string
		curFileName.clear();

		// Go to the start of file name
		pos = fileNameStart[i];

		// Copy every char into the string until you encounter 00
		while (cacheFileBytes[pos] != 0)
		{
			curFileName += cacheFileBytes[pos];

			// If we encounter a directory, create it.
			// Platform dependent (Windows)
			if (cacheFileBytes[pos] == '/')
			{
				path = baseDir + "/" + curFileName;

				if (!CreateDirectory(s2ws(path).c_str(), NULL) && ERROR_ALREADY_EXISTS != GetLastError())
				{
					std::cout << "Error creating directory " << path << "/n";
					return 1;
				}
			}

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

		// Go to the start of file content
		pos = fileContentStart[i];

		// Copy fileSize bytes into curFileBytes.
		for (int j = 0; j < fileSize[i]; j++)
		{
			curFileBytes.push_back((unsigned char)cacheFileBytes[pos]);
			pos++;
		}

		// Get a full path to the file we want to create
		path = baseDir + "/" + fileName[i];

		// Create new file with name fileName that consists of every copied byte stored in curFileBytes. Save the file.
		out_file.open(path, std::ios::out | std::ios::binary | std::ios::trunc);
		for (int j = 0; j < fileSize[i]; j++)
			out_file << curFileBytes[j];

		out_file.close();
	}

	return 0;
}

int repack(char cacheName[], char directory[])
{
	std::string cache_old(cacheName);
	std::string new_files(directory);
	char* cacheFileBytes;

	//int fileCount;

	/*TODO:
	1) Open cacheName
	2) Get old attributes of cacheName: fileCount, fileSize, fileNameStart, fileContentStart, secretBytesStart, secretBytesContent
	3) If the file count in directory is different than in the cache, return error
	4) Get the list of files from directory, remove directory path from names
	5) If a file path doesn't fit the one in the cache, return error
	6) Modify parameters as such:
		a) if file's size is different: offset every fileContentStart after the file and secretBytesStart with the difference
		b) if file's name is different: offset every fileNameStart after the file, every fileContentStart and 
	*/


	// Copy bytes from cache file into cacheFileBytes
	//cacheFileBytes = FileIntoByteVector(cacheName);

	

	std::cout << "You chose to repack!\n";

	return 0;
}

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		std::cout << "Usage:\n";
		std::cout << "1) Extraction: PPQCacheExtractor.exe file_path_1 file_path_2 ...\n";
		std::cout << "2) Repacking: PPQCacheExtractor.exe -r cache_file directory_with_new_files\n";
		return 0;
	}

	// Check if the parameter for repacking is passed
	std::string arg1(argv[1]);
	if (arg1 == "-r")
	{
		repack(argv[2], argv[3]);
		return 0;
	}

	for (int i = 1; i < argc; i++)
	{
		if (extract(argv[i]) == 0)
		{
			std::cout << "Successfully extracted file " << argv[i] << std::endl;
		}
	}

	return 0;
}
