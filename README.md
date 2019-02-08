# PPQCacheExtractor
The main purpose of this project is to reverse engineer downloaded/cache files of a PC version of a F2P game Puyo Puyo Quest!! avaliable on DMM platform in order to enable extracting and, in future, replacing the files.

## Prerequisites
 * Microsoft Visual C++ 2017 Redistributable
 * An environment that supports C++ for compiling (For compiling on other platforms than Windows you may have to remove "pch.h" header from the project)
 * For testing purposes you have to download the PC version of PPQ. The default installation location is:
```
C:\Users\[user-name]\PuyopuyoQuest
```
The cache files are located in this folder:
```
[root-PPQ-folder]\caches\downloads
```

## Downloading the prebuilt file
Check releases for newest versions of the project.

## Running
Using the command line/terminal pass the name of the file you want to extract like this:
```
.\PPQCacheExtractor.exe .\00000011
```
Alternatively you can drag and drop the file you want to extract to the executable.

The program will extract the files to the directory you were when you called it.

## Built With
Microsoft Visual Studio Community.

## Contributing
In case you want to help first write the code for features listed in TODO. Don't heavily change the structure of the program without providing a good reason.

If you have an idea for a feature, please discuss!
