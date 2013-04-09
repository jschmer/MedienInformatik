#ifndef CONFIG_FILE_H__
#define CONFIG_FILE_H__

#include <string>
#include <map>
#include <vector>
#include <functional>
#include <windows.h>
#include <iostream>
#include <thread>
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

    std::map   <string, string>       content_;
    std::vector<string>               sections_;
	std::function<void (ConfigFile&)> callback_;

	// for monitoring thread
    std::thread monitorThread;
    bool stopMonitor;
	FW::FileWatcher fileWatcher;

public:
	ConfigFile(string const& configFile, char const delim = '=');
	~ConfigFile();

	// logical I/O operation
	string get(string section, string key);
	string get(string section, string key, string def);

	// stuff
	string getPath();
    std::vector<string> GetSections();

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
		callback_(*this);
	}
};

#endif 
