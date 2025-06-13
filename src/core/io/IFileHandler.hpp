#ifndef _FILE_HANDLER_HPP_
#define _FILE_HANDLER_HPP_
#include <filesystem>
namespace dragon
{
	class FileHandler
	{
	public: 
		virtual ~FileHandler() = default; 
		virtual void open(const std::filesystem::path& file_path) = 0;
	};
}
#endif // !_FILE_HANDLER_HPP_
