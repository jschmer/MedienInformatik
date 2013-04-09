#ifndef CONVERTHELPER_H
#define CONVERTHELPER_H

#include <iomanip>
#include <sstream>
#include <string>

class InvalidConversion {};

namespace ConvertHelper {

	/*
	 * Convert to arbitrary type
	 */
	// taken from http://www.parashift.com/c++-faq-lite/convert-string-to-num.html
	template <typename T>
	T To ( std::string const &Text ) {
		std::stringstream ss;
		ss << Text;
		T result;

		if (!(ss >> result))
			throw InvalidConversion();

		return result;
	}

	template <>
	inline bool To<bool> ( std::string const &Text ) {
		bool result;

		if (Text == "true" || Text == "false")
			result = (Text == "true" ? true : false);
		else
			throw InvalidConversion();

		return result;
	}

	template <>
	inline std::string To<std::string> ( std::string const &Text ) {
		return Text;
	}

	/*
	 * Convert to CString
	 */
	// Taken from http://www.parashift.com/c++-faq-lite/convert-num-to-string.html
	template <typename T>
	std::string ToString ( T Number, unsigned int precision = 10 ) {
		std::ostringstream ss;
		if (!(ss << std::setprecision(precision) << Number))
			throw InvalidConversion();
		return ss.str().c_str();
	}

	template <>
	inline std::string ToString<bool> ( bool boolean, unsigned int precision ) {
		std::string result = boolean ? "true" : "false";
		return result;
	}
}

#endif