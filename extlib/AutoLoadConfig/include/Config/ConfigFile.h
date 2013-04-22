/* 
 * Copyright (c) 2013 Jens Schmer
 * This file is distributed under the MIT License.
 * Consult COPYING within this package for further information. 
 */
#ifndef CONFIG_FILE_H__
#define CONFIG_FILE_H__

#include <string>
#include <map>
#include <set>
#include <vector>
#include <functional>
#include <windows.h>
#include <iostream>
#include <thread>
#include <mutex>
#include <FileWatcher/FileWatcher.h>

using std::string;

class ConfigFile {
	class Listener : public FW::FileWatchListener {
		ConfigFile& cfg;
	
    public:
		Listener(ConfigFile& cfg) : cfg(cfg) {}
		void handleFileAction(	FW::WatchID watchid,
								const FW::String& dir,
								const FW::String& filename,
								FW::Action action)
		{
			string FWevent;

			switch (action) {
				case FW::Action::Add:
					FWevent = "Add";
					break;
				case FW::Action::Delete:
					FWevent = "Delete";
					break;
				case FW::Action::Modified:
					FWevent = "Modified";
					break;
				default:
					break;
			}
			std::cout << "DIR (" << dir + ") FILE (" + filename + ") has event \"" << FWevent << "\"" << std::endl;

			// if this config file was affected
			if (filename == cfg.configFileName_) {
				cfg.readConfig();
				cfg.runCallback();
			}
		}
	};

	char   delimiter_;
	string pathToConfigFile_;
	string configFileName_;

    // data
    std::map<string, string>          content_;
    std::set<string>                  sections_;

    // callback on file update
	std::function<void (ConfigFile&)> callback_;
    bool do_callback_;

	// for monitoring thread
    std::thread monitorThread;
    std::mutex mtx;
    bool stopMonitor;
	FW::FileWatcher fileWatcher;

public:
	ConfigFile(string const& configFile, char const delim = '=');
	~ConfigFile();

    // logical I/O operation
	string get(string section, string key);
	string get(string section, string key, string def);
    void put(string section, string key, string value);

	// stuff
	string getPath();
    std::set<string> GetSections();
    void disableCallback() { do_callback_ = false; }
    void enableCallback() { do_callback_ = true; }

	// callback stuff
	void setCallbackOnUpdate(std::function<void (ConfigFile&)> callback) {
		callback_ = callback;
	}

	void monitorChanges();

private:
	// physical I/O operation (on disk)
	void readConfig();
	void writeConfig();

	// callback stuff
	void runCallback() {
        if (do_callback_)
		    callback_(*this);
	}
};

#endif 
