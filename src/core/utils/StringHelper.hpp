#ifndef _STRING_HELPER_HPP_
#define _STRING_HELPER_HPP_
#include <string>
#include <algorithm>
#include <cctype>
#include <fstream>
#include "spdlog/spdlog.h"
namespace dragon
{
	class StringHelper
	{
	public:
		static std::string tolower(std::string s)
		{
			std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c); });
			return s;
		}
		static std::string ReadFile(std::string filename)
		{
			spdlog::info("read file : {}", filename);
			std::ifstream t(filename);
			std::stringstream buffer;
			buffer << t.rdbuf();
			return buffer.str();
		}
	private:
		StringHelper();
	};
}
#endif // !_STRING_HELPER_HPP_