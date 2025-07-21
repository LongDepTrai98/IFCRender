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
		static bool readFile(const std::string& path, std::string& content)
		{
			std::ifstream file;
			file.open(path, std::ifstream::ate);
			char* contents;
			if (!file) {
				contents = new char[1];
				content = std::string(contents);
				delete[]contents;
				return false;
			}
			size_t file_size = file.tellg();
			file.seekg(0);
			std::filebuf* file_buf = file.rdbuf();
			contents = new char[file_size + 1]();
			file_buf->sgetn(contents, file_size);
			file.close();
			content = std::string(contents);
			delete[]contents;
			return true;
		}
		static bool loadBinaryFile(const std::string& path,
			std::string* content)
		{
			std::ifstream ifs(path, std::ifstream::binary);
			if (!ifs)
			{
				return false;
			}
			// The fastest way to read a file into a string.
			ifs.seekg(0, std::ios::end);
			auto size = ifs.tellg();
			(*content).resize(static_cast<size_t>(size));
			ifs.seekg(0, std::ios::beg);
			ifs.read(&(*content)[0], (*content).size());
			return !ifs.bad();
		}
	private:
		StringHelper();
	};
}
#endif // !_STRING_HELPER_HPP_