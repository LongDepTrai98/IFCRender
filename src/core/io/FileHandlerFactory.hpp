#ifndef _FILE_HANLDER_FACTORY_HPP_
#define _FILE_HANLDER_FACTORY_HPP_
#include <filesystem>
#include <memory>
namespace dragon
{
	class FileHandler; 
	class FileHandlerFactory
	{
	public: 
		static std::unique_ptr<FileHandler> create(const std::filesystem::path& file); 
	private: 
		FileHandlerFactory();
	};
}
#endif // !_FILE_HANLDER_FACTORY_HPP_
