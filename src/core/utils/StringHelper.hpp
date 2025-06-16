#ifndef _STRING_HELPER_HPP_
#define _STRING_HELPER_HPP_
#include <string>
#include <algorithm>
#include <cctype>
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
	private: 
		StringHelper(); 
	};
}
#endif // !_STRING_HELPER_HPP_
