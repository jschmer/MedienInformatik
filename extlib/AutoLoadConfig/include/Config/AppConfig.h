#ifndef APPCONFIG_H
#define APPCONFIG_H

#include "ConfigFile.h"

#include <String/ConvertHelper.h>
#include <string>

using std::string;

/** 
 * @class	AppConfig 
 * @author	schmer
 * @brief	Provides an interface for writing and reading C++ datatypes to a string based ConfigFile
 */
class AppConfig {
public:
	AppConfig(ConfigFile *configFile);
	~AppConfig();

	/**
	 * @fn			AppConfig::write(string key, InputType value)
	 * @brief		Writes the value to the associated key
	 * @param[in]	key			key of the value to write
	 * @param[in]	value		value to write to the key
	 */
	template<typename InputType>
	void write(string key, InputType value) {
		// convert value to string
		std::stringstream converter;
		converter << ConvertHelper::Tostring<InputType>(value);

		// write value as a string to config file
		configFile->put(key, converter.str().c_str());
	}

	/**
	 * @fn			AppConfig::read(string key, OutputType def)
	 * @brief		Reads the value associated with key and converts it to the specified type (OutputType)
	 * @param[in]	key			key of the value to read
	 * @param[in]	def			default value to return if the conversion fails
	 * @returns		converted value with specified OutputType\n
	 *				or the default value 'def' if the conversion failed
	 */
	template<typename OutputType>
	OutputType read(string key, OutputType def) {
		// read value from config file
		string value(configFile->get(key));
		OutputType converted;

		// convert the value
		try {
			converted = ConvertHelper::To<OutputType>(value);
		} catch(InvalidConversion) {
			converted = def;
		}
		
		return converted;
	}

private:
	ConfigFile *configFile;
};

#endif