#include "ConfigFile.h"

#include <process.h>
#include <direct.h>
#define GetCurrentDir _getcwd

#include <iostream>
#include <fstream>

#include <Win/Helper.h>
#include <String/StringHelper.h>

ConfigFile::ConfigFile(std::string const& configFile, char const delim)
	:	delimiter_(delim),
		stopMonitor(false),
        monitorThread()
{
	// get current working dir
	char cCurrentPath[FILENAME_MAX];
	if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath))) {
		std::cout << "\n ERROR: GetCurrentDir function failed. errno: " << errno << std::endl;
	}
	string workingDir = cCurrentPath;
	string cfgFile    = configFile;

	// use only forward slash for paths
	StringHelper::replace(cfgFile, "\\", "/");
	StringHelper::replace(workingDir, "\\", "/");

	// split path and filename
	auto pos = cfgFile.find_last_of("/");
	if (pos != string::npos) {
		pathToConfigFile_ = workingDir + "/" + cfgFile.substr(0, pos) + "/";
		configFileName_   = cfgFile.substr(pos+1);
	} else {
		pathToConfigFile_ = workingDir + "/";
		configFileName_   = cfgFile;
	}

	// read the config file
	readConfig();
}

ConfigFile::~ConfigFile() {
	stopMonitor = true;
    monitorThread.join();
}

void ConfigFile::readConfig() {
	using StringHelper::trim;

	std::ifstream file(pathToConfigFile_ + configFileName_);

	std::string inSection, line;
	while (std::getline(file,line)) {
		if (!line.length()) continue;
		if (line[0] == '#') continue;
		if (line[0] == ';') continue;
		if (line[0] == '[') {
			inSection = trim(line.substr(1, line.find(']')-1));
			sections_.push_back(inSection);
			continue;
		}

		int posEqual = line.find('=');
		std::string name  = trim(line.substr(0, posEqual));
		std::string value = trim(line.substr(posEqual+1));

		content_[inSection+'/'+name] = value;
	}
}

string ConfigFile::get(string section, string key) {
	std::map<std::string, std::string>::const_iterator ci = content_.find(section + '/' + key);

	if (ci == content_.end())
		throw std::out_of_range("Key not found: " + section + "/" + key);

	return ci->second;
}

string ConfigFile::get(string section, string key, string def) {
	try {
		return get(section, key);
	} catch(std::out_of_range) {
		return content_.insert(std::make_pair(section+'/'+key, def)).first->second;
	}
}

string ConfigFile::getPath() {
	return pathToConfigFile_ + configFileName_;
}

std::vector<std::string> ConfigFile::GetSections() {
	return sections_;
}

void ConfigFile::monitorChanges() {
	std::cout << "Start monitoring " << pathToConfigFile_ << std::endl;

	// Create the monitor thread.
    monitorThread = std::thread([&]() {
        // start filewatcher: add a watch to the system
        try {
            FW::WatchID watchID = fileWatcher.addWatch(pathToConfigFile_, new ConfigFile::Listener(*this));
        } catch (FW::FileNotFoundException& e) {
            std::cout << "\n ERROR: " << e.what();
            return;
        }

        // loop until stop is signaled
        while(!stopMonitor)
        {
            fileWatcher.update();
            Sleep(1);
        }

        std::cout << "Monitoring stopped..." << std::endl;
    });
}