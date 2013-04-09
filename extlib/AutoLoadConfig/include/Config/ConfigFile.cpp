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
        monitorThread(),
        do_callback_(false),
        mtx()
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
    if (monitorThread.joinable())
        monitorThread.join();
}

void ConfigFile::readConfig() {
    std::lock_guard<std::mutex> guard(mtx);
	using StringHelper::trim;

	std::ifstream file(pathToConfigFile_ + configFileName_);

	std::string inSection, line;
	while (std::getline(file,line)) {
		if (!line.length()) continue;
		if (line[0] == '#') continue;
		if (line[0] == ';') continue;

        // handle section header
		if (line[0] == '[') {
			inSection = trim(line.substr(1, line.find(']')-1));
			sections_.insert(inSection);
			continue;
		}

        // split key/value pair
		int posEqual = line.find('=');
		std::string name  = trim(line.substr(0, posEqual));
		std::string value = trim(line.substr(posEqual+1));

        // add to internal representation
		content_[inSection+'/'+name] = value;
	}
}

void ConfigFile::writeConfig() {
    std::lock_guard<std::mutex> guard(mtx);
	using StringHelper::trim;
    do_callback_ = false;

	std::ifstream file(pathToConfigFile_ + configFileName_);

    auto content_copy = content_;

	std::string inSection, line, lineoutput;
	while (std::getline(file,line)) {
		if (!line.length()) {
            lineoutput += "\n";
            continue;
        }
		if (line[0] == '#') {
            lineoutput += line + "\n";
            continue;
        }
		if (line[0] == ';') {
            lineoutput += line + "\n";
            continue;
        }
		if (line[0] == '[') {
            lineoutput += line + "\n";
			inSection = trim(line.substr(1, line.find(']')-1));
			continue;
		}

		int posEqual = line.find(delimiter_);
		std::string name  = trim(line.substr(0, posEqual));
		std::string value = trim(line.substr(posEqual+1));

        auto internal_value = get(inSection, name, "not_in_map");

        // if internal_value differs from value in config: replace it!
        if (value != internal_value) {
            auto pos = line.find(value);
            if (pos != string::npos) {
                line = line.replace(pos, value.length(), internal_value);
            }
        }

        content_copy.erase(inSection + "/" + name);
        lineoutput += line + "\n";
	}

    // insert all data that is left over
    for (const auto& pair : content_copy) {
        auto key = pair.first;
        auto val = pair.second;

        // get section and name
        auto split = StringHelper::split(key, '/');

        auto section = split[0];
        auto name = split[1];
        auto name_value = name + " " + delimiter_ + " " + val;

        if (section == "") {
            // prepend new name/value pair
            lineoutput = name_value + "\n" + lineoutput;
        }
        else {
            // find section begin
            auto pos = lineoutput.find(section);
            if (pos != std::string::npos) {
                // section already exist, append new name/value pair
                pos += section.size() + 2; // jumping over newline...
                lineoutput.insert(pos, name_value + "\n");
            }
            else {
                // section does not exist, append new section and name/value pair
                lineoutput += "\n[" + section + "]\n" + name_value + "\n";
            }
        }
    }

    file.close();

    // write buffer
    std::ofstream ofile(pathToConfigFile_ + configFileName_, std::ofstream::trunc);
    ofile << lineoutput;
    ofile.close();

    do_callback_ = true;
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
        // key does not exist, insert default value
        put(section, key, def);
		return def;
	}
}

void ConfigFile::put(string section, string key, string value) {
    sections_.insert(section);
    content_[section+'/'+key] = value;

    writeConfig();
}

string ConfigFile::getPath() {
	return pathToConfigFile_ + configFileName_;
}

std::set<std::string> ConfigFile::GetSections() {
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