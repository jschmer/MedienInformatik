/* 
 * Copyright (c) 2013 Jens Schmer
 * This file is distributed under the MIT License.
 * Consult COPYING within this package for further information. 
 */
#ifndef STRINGHELPER_H
#define STRINGHELPER_H

#include <string>
#include <vector>
#include <sstream>

namespace StringHelper {
	inline std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
		std::stringstream ss(s);
		std::string item;
		while(std::getline(ss, item, delim)) {
			elems.push_back(item);
		}
		return elems;
	}

	inline std::vector<std::string> split(const std::string &s, char delim) {
		std::vector<std::string> elems;
		return split(s, delim, elems);
	}

	void replace(std::string& str, const std::string& oldStr, const std::string& newStr)
	{
		size_t pos = 0;
		while((pos = str.find(oldStr, pos)) != std::string::npos)
		{
			str.replace(pos, oldStr.length(), newStr);
			pos += newStr.length();
		}
	}

	std::string trim(std::string const& source, char const* delims = " \t\r\n") {
		std::string result(source);

		std::string::size_type index = result.find_last_not_of(delims);
		if(index != std::string::npos)
			result.erase(++index);

		index = result.find_first_not_of(delims);
		if(index != std::string::npos)
			result.erase(0, index);
		else
			result.erase();

		return result;
	}

}
#endif