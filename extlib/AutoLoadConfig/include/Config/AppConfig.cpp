#include "AppConfig.h"
#include <sstream>

AppConfig::AppConfig(ConfigFile *confFile)
	: configFile(confFile)
{}

AppConfig::~AppConfig() {
	delete configFile;
}