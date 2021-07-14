# PPQCacheExtractor
The main purpose of this project is to reverse engineer downloaded/cache files of a F2P game Puyo Puyo Quest!! in order to enable extracting and experimental replacing the files. While originally developed based off the now defunct PC version formerly available on DMM, it will also work with files from the Android version.

## Prerequisites
 * Microsoft Visual C++ 2019 Redistributable
 * Windows SDK 10.0
 * Rooted phone with Puyo Puyo Quest!! installed (to access files)

## Downloading the prebuilt file
Check releases for newest versions of the project.

## Running
Using the command line pass the name of the file(s) you want to extract like this, or drag and drop into the executable:
```
.\PPQCacheExtractor.exe .\00000011
```
This will create folders with _out suffix for each cache file you input.

For extracting use the command line to pass the file you want to rebuild and folder containing modified files like this:
```
.\PPQCacheExtractor.exe -r .\00000011 .\folder_with_new_files
```
This will create a new file with _new suffix. Keep in mind using the new file will most likely crash the game when it tries loading in modified files.

## Built With
Microsoft Visual Studio Community 2019.

## Contributing
There are two things you can do to help the project. Figure out why the game crashes when using modified files and what the final 4 bytes really mean.

If you have an idea for a feature, please discuss!
