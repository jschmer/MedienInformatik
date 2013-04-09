#include <Windows.h>

#include <string>
#include <fstream>
#include <iostream>
#include <vector>

#include <String/StringHelper.h>

using std::string;

static bool IsDirectory(string const &strPath) {
	DWORD dwAttrib	= GetFileAttributesA( strPath.c_str() );
	return static_cast<bool>( ( dwAttrib != 0xffffffff 
							&&	(dwAttrib & FILE_ATTRIBUTE_DIRECTORY)) );

		
}

bool fileExists(std::string pathToFile) {
	std::ifstream ifile(pathToFile);
	bool g = ifile.good();
	return g;
}

bool CreateSubTreeForConfigFile(string path) {
	if (path.length() <= 0)
		// all ok, file is in current directory
		return true;
	else {
		// make sure that all subdirectories exist
		std::vector<string> subdirs = StringHelper::split(string(path), '/');
		string subpath = "";

		// construct subpath from bottom up, adding one directory each loop
		for (std::vector<string>::iterator it = subdirs.begin(); it != subdirs.end(); ++it) {
			subpath += *it + "/";

			if (CreateDirectoryA(subpath.c_str(), NULL) == FALSE)
			{
				if (ERROR_ALREADY_EXISTS == GetLastError())
					continue;

				std::cout << "Failed to create " << subpath << std::endl;
				return false;
			}
		}
	}
	return true;
}