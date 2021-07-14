// PPQCacheExtractor.cpp : Ten plik zawiera funkcję „main”. W nim rozpoczyna się i kończy wykonywanie programu.
//

#include "pch.h"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <Windows.h>

using namespace std;

// Opens file in binary mode and extracts bytes into char array
char* FileIntoByteVector(char* filename)
{
	ifstream ifs(filename, ios::binary);

	if (!ifs.is_open())
	{
		cout << "Error opening file " << filename << endl;
		return NULL;
	}

	ifs.seekg(0, ifs.end);
	int length = ifs.tellg();
	char* result = new char[length];
	
	ifs.seekg(0, ifs.beg);
	ifs.read(result, length);

	return result;
}

char* FileIntoByteVector(string filename, unsigned int* fileSize)
{
	ifstream ifs(filename, ios::binary);

	if (!ifs.is_open())
	{
		cout << "Error opening file " << filename << endl;
		return NULL;
	}

	ifs.seekg(0, ifs.end);
	*fileSize = ifs.tellg();
	char* result = new char[*fileSize];

	ifs.seekg(0, ifs.beg);
	ifs.read(result, *fileSize);

	return result;
}

// Converts int into a byte array
vector<unsigned char> IntIntoByteArray(int param)
{
	vector<unsigned char> byteArray(4);

	for (int i = 0; i < 4; i++)
		byteArray[i] = param >> (i * 8);

	return byteArray;
}

// Converting string so it can be used in CreateDirectory
wstring StringIntoWstring(string s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	wstring r(buf);
	delete[] buf;
	return r;
}

// Gets header data from the opened cache file
void GetHeaderData(char* cacheFileBytes, unsigned int* fileCount, vector<unsigned int>* fileSize, vector<unsigned int>* fileNameStart, vector<unsigned int>* fileContentStart)
{
	// Get first 4 bytes and store them in fileCount
	*fileCount = (unsigned int)((unsigned char)cacheFileBytes[0] | (unsigned char)cacheFileBytes[1] << 8 | (unsigned char)cacheFileBytes[1] << 16 | (unsigned char)cacheFileBytes[1] << 24);

	// Starting from byte 4 fill the fileSize vector with file sizes read as 4 bytes fileCount times.
	int pos = 4;
	for (unsigned int i = 0; i < *fileCount; i++)
	{
		fileSize->push_back((unsigned int)((unsigned char)cacheFileBytes[pos + i * 4] | (unsigned char)cacheFileBytes[pos + i * 4 + 1] << 8
			| (unsigned char)cacheFileBytes[pos + i * 4 + 2] << 16 | (unsigned char)cacheFileBytes[pos + i * 4 + 3] << 24));
	}

	// Read fileCount vaules and assign them to fileNameStart
	pos += *fileCount * 4;
	for (unsigned int i = 0; i < *fileCount; i++)
	{
		fileNameStart->push_back((unsigned int)((unsigned char)cacheFileBytes[pos + i * 4] | (unsigned char)cacheFileBytes[pos + i * 4 + 1] << 8
			| (unsigned char)cacheFileBytes[pos + i * 4 + 2] << 16 | (unsigned char)cacheFileBytes[pos + i * 4 + 3] << 24));
	}

	// Read fileCount vaules and assign them to fileContentStart
	pos += *fileCount * 4;
	for (unsigned int i = 0; i < *fileCount; i++)
	{
		fileContentStart->push_back((unsigned int)((unsigned char)cacheFileBytes[pos + i * 4] | (unsigned char)cacheFileBytes[pos + i * 4 + 1] << 8
			| (unsigned char)cacheFileBytes[pos + i * 4 + 2] << 16 | (unsigned char)cacheFileBytes[pos + i * 4 + 3] << 24));
	}
}

// Gets file names from the opened cache file
void GetFileNames(char* cacheFileBytes, unsigned int fileCount, vector<unsigned int> fileNameStart, vector<string>* fileName)
{
	string curFileName;
	string path;
	int pos;
	// Find the file names and put them in fileName vector
	for (unsigned int i = 0; i < fileCount; i++)
	{
		// Reset the file name string
		curFileName.clear();

		// Go to the start of file name
		pos = fileNameStart[i];

		// Copy every char into the string until you encounter 00
		while (cacheFileBytes[pos] != 0)
		{
			curFileName += cacheFileBytes[pos];
			pos++;
		}

		// Insert the filename into fileName vector
		curFileName += '\0';
		fileName->push_back(curFileName);
	}
}

// Gets file contents from the cache file
void GetFileContents(char* cacheFileBytes, unsigned int fileCount, vector<unsigned int> fileSize, vector<unsigned int> fileContentStart, vector<vector<unsigned char>>* fileContent)
{
	int pos;
	for (unsigned int i = 0; i < fileCount; i++)
	{
		// Go to the start of file content
		pos = fileContentStart[i];

		vector<unsigned char> currentFileBytes;

		// Copy fileSize bytes into fileBytes.
		for (unsigned int j = 0; j < fileSize[i]; j++)
		{
			currentFileBytes.push_back((unsigned char)cacheFileBytes[pos]);
			pos++;
		}

		fileContent->push_back(currentFileBytes);
	}
}

int Extract(char cacheName[])
{
	char* cacheFileBytes;

	unsigned int fileCount;
	vector<unsigned int> fileSize;
	vector<unsigned int> fileNameStart;
	vector<unsigned int> fileContentStart;
	vector<string> fileName;
	vector<vector<unsigned char>> fileContent;

	string cacheDir(cacheName);

	// Try opening the file
	// Copy bytes from cache file into cacheFileBytes
	cacheFileBytes = FileIntoByteVector(cacheName);

	if (cacheFileBytes == NULL)
	{
		cout << "Error opening cache file " << cacheDir;
		return -1;
	}

	cacheDir += "_out/";

	// Create the output directory. If it fails not because it already exists, error, end
	// Platform dependent rn (Windows).
	if (!CreateDirectory(StringIntoWstring(cacheDir).c_str(), NULL) && ERROR_ALREADY_EXISTS != GetLastError())
	{
		cout << "Error creating directory " << cacheDir << "\n";
		return -1;
	}

	// Get data from header
	GetHeaderData(cacheFileBytes, &fileCount, &fileSize, &fileNameStart, &fileContentStart);

	// Get file names
	GetFileNames(cacheFileBytes, fileCount, fileNameStart, &fileName);

	// Get file contents
	GetFileContents(cacheFileBytes, fileCount, fileSize, fileContentStart, &fileContent);

	ofstream out_file;
	string path;

	for (unsigned int i = 0; i < fileCount; i++)
	{
		// If file name contains a directory, create it
		for (int j = 0; j < fileName[i].length(); j++)
		{
			if (fileName[i][j] == '/')
			{
				path = cacheDir + "/" + fileName[i].substr(0, j + 1);

				if (!CreateDirectory(StringIntoWstring(path).c_str(), NULL) && ERROR_ALREADY_EXISTS != GetLastError())
				{
					cout << "Error creating directory " << path << "/n";
					return -1;
				}
			}
		}

		// Get a full path to the file we want to create
		path = cacheDir + "/" + fileName[i];

		// Create new file with name fileName that consists of every copied byte stored in curFileBytes. Save the file.
		out_file.open(path, ios::out | ios::binary | ios::trunc);
		for (unsigned int j = 0; j < fileSize[i]; j++)
			out_file << fileContent[i][j];

		out_file.close();
	}

	return 0;
}

int Repack(char cacheName[], char directory[])
{
	char* cacheFileBytes;

	unsigned int fileCount;
	vector<unsigned int> fileSize;
	vector<unsigned int> fileNameStart;
	vector<unsigned int> fileContentStart;
	vector<string> fileName;
	unsigned int unknownBytes;

	string cacheDir(cacheName);
	string injectingDir(directory);

	// Try opening the file
	cacheFileBytes = FileIntoByteVector(cacheName);

	if (cacheFileBytes == NULL)
	{
		cout << "Error opening cache file " << cacheDir;
		return -1;
	}

	// Get data from header
	GetHeaderData(cacheFileBytes, &fileCount, &fileSize, &fileNameStart, &fileContentStart);

	// Get file names
	GetFileNames(cacheFileBytes, fileCount, fileNameStart, &fileName);

	unsigned int unknownBytesStart = fileContentStart[fileCount - 1] + fileSize[fileCount - 1];

	if (unknownBytesStart % 4 != 0)
		unknownBytesStart += 4 - (unknownBytesStart % 4);

	unknownBytes = (unsigned int)((unsigned char)cacheFileBytes[unknownBytesStart] | (unsigned char)cacheFileBytes[unknownBytesStart + 1] << 8
		| (unsigned char)cacheFileBytes[unknownBytesStart + 2] << 16 | (unsigned char)cacheFileBytes[unknownBytesStart + 3] << 24);

	// Initialize new vectors for files, their sizes and beginning points
	vector<unsigned int> fileSizeNew = fileSize;
	vector<unsigned int> fileContentStartNew = fileContentStart;
	vector<char *> fileContentNew;
	string path;

	// Read new files and save their contents
	for (int i = 0; i < fileCount; i++)
	{
		path = injectingDir + "/" + fileName[i];

		char* openedFile = FileIntoByteVector(path, &fileSizeNew[i]);

		if (openedFile == NULL)
		{
			cout << "Error opening file " << path;
			return -1;
		}

		fileContentNew.push_back(openedFile);

		// Calculate difference between old and new
		unsigned int fileSizeDelta = fileSizeNew[i] - fileSize[i];

		// Apply the difference into fileContentStartNew and make it divisible by 4
		for (int j = i + 1; j < fileCount; j++)
		{
			fileContentStartNew[j] += fileSizeDelta;

			if (fileContentStartNew[j] % 4 != 0)
				fileContentStartNew[j] += 4 - (fileContentStartNew[j] % 4);
		}
	}

	// Set new start for unknown bytes
	unsigned int unknownBytesStartNew = fileContentStartNew[fileCount - 1] + fileSizeNew[fileCount - 1];
	if (unknownBytesStartNew % 4 != 0)
		unknownBytesStartNew += 4 - (unknownBytesStartNew % 4);

	// Build new cache file
	vector<unsigned char> cacheContent;
	unsigned int pos = 0;

	// Header
	// File Count
	vector<unsigned char> byteArray = IntIntoByteArray(fileCount);
	cacheContent.push_back(byteArray[0]);
	cacheContent.push_back(byteArray[1]);
	cacheContent.push_back(byteArray[2]);
	cacheContent.push_back(byteArray[3]);
	pos += 4;

	// File Size
	for (int i = 0; i < fileCount; i++)
	{
		byteArray = IntIntoByteArray(fileSizeNew[i]);
		cacheContent.push_back(byteArray[0]);
		cacheContent.push_back(byteArray[1]);
		cacheContent.push_back(byteArray[2]);
		cacheContent.push_back(byteArray[3]);
		pos += 4;
	}

	// File Name Start
	for (int i = 0; i < fileCount; i++)
	{
		byteArray = IntIntoByteArray(fileNameStart[i]);
		cacheContent.push_back(byteArray[0]);
		cacheContent.push_back(byteArray[1]);
		cacheContent.push_back(byteArray[2]);
		cacheContent.push_back(byteArray[3]);
		pos += 4;
	}

	// File Content Start
	for (int i = 0; i < fileCount; i++)
	{
		byteArray = IntIntoByteArray(fileContentStartNew[i]);
		cacheContent.push_back(byteArray[0]);
		cacheContent.push_back(byteArray[1]);
		cacheContent.push_back(byteArray[2]);
		cacheContent.push_back(byteArray[3]);
		pos += 4;
	}

	// File Names
	for (int i = 0; i < fileCount; i++)
	{
		//if pos doesn't match fileNameStart, add 0s until it does
		for (int j = pos; j < fileNameStart[i]; j++)
		{
			cacheContent.push_back(0);
			pos++;
		}
		
		//insert chars until we run into the end
		for (int j = 0; j < fileName[i].length(); j++)
		{
			cacheContent.push_back(fileName[i][j]);
			pos++;
		}
		
	}

	// File Contents
	for (int i = 0; i < fileCount; i++)
	{
		//if pos doesn't match fileContentStartNew, add 0s until it does
		for (int j = pos; j < fileContentStartNew[i]; j++)
		{
			cacheContent.push_back(0);
			pos++;
		}

		// insert bytes until we finish
		for (unsigned int j = 0; j < fileSizeNew[i]; j++)
		{
			cacheContent.push_back(fileContentNew[i][j]);
			pos++;
		}
	}

	// Unknown bytes
	//if pos doesn't match fileContentStartNew, add 0s until it does
	for (int j = pos; j < unknownBytesStartNew; j++)
	{
		cacheContent.push_back(0);
		pos++;
	}

	byteArray = IntIntoByteArray(unknownBytes);
	cacheContent.push_back(byteArray[0]);
	cacheContent.push_back(byteArray[1]);
	cacheContent.push_back(byteArray[2]);
	cacheContent.push_back(byteArray[3]);
	pos += 4;

	/*
	TODO:
	5. Build new cache file:
		-header
		-file names
		-file contents - fill gaps with 00
		-unknownBytes - put them in next address divisible by 4
	*/

	// Save cache file
	ofstream out_file;

	out_file.open(cacheDir + "_new", ios::out | ios::binary | ios::trunc);

	for (unsigned int i = 0; i < unknownBytesStartNew + 4; i++)
		out_file << cacheContent[i];

	out_file.close();

	return 0;
}

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		cout << "Usage:\n";
		cout << "1) Extraction: PPQCacheExtractor.exe file_path_1 file_path_2 ...\n";
		cout << "2) Repacking: PPQCacheExtractor.exe -r cache_file directory_with_new_files\n";
		return 0;
	}

	// Check if the parameter for repacking is passed
	string arg1(argv[1]);
	if (arg1 == "-r")
	{
		if (Repack(argv[2], argv[3]) == 0)
			cout << "Successfully repacked file " << argv[2] << " into " << argv[2] << "_new";
		return 0;
	}

	for (int i = 1; i < argc; i++)
	{
		if (Extract(argv[i]) == 0)
		{
			cout << "Successfully extracted file " << argv[i] << " into " << argv[i] << "_out" << endl;
		}
	}

	return 0;
}
