#include <cctype>
#include <cstdarg>
#include <sstream>
#include "utils.h"

namespace bbk
{
namespace utils
{
int xatoi(const char *str)
{
	while (std::isspace(*str))
		++str;

	bool isPos = true;
	if (*str == '-')
		isPos = false;

	int result = 0;
	while (std::isxdigit(*str))
	{
		if (std::isdigit(*str))
			result = result * 16 + (*str++ - '0');
		else
			result = result * 16 + (std::toupper(*str++) - 'A' + 10);
	}
	return isPos ? result : -result;
}

std::string VarArgToStr(const char *format, ...)
{
	std::va_list argList;
	va_start(argList, format);

	char buffer[1024] = {0};
	std::vsprintf(buffer, format, argList);
	va_end(argList);

	return std::string(buffer);
}

std::vector<std::string>& SplitStrToWords(const std::string &origStr, char delimiter, std::vector< std::string > &outputWordsVec)
{
	std::stringstream ss(origStr);
	std::string       token;

	while (std::getline(ss, token, delimiter))
		outputWordsVec.push_back(token);

	return outputWordsVec;
}

std::vector<std::string> SplitStrToWords(const std::string &origStr, char delimiter)
{
	std::vector<std::string> outputWordsVec;
	SplitStrToWords(origStr, delimiter, outputWordsVec);
	return outputWordsVec;
}
} // namespace utils
} // namespace bbk
