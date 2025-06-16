#include "FileHandlerFactory.hpp"
#include "IFCFileHandler.hpp"
namespace dragon
{
	std::unique_ptr<FileHandler> FileHandlerFactory::createFileHandler(const std::filesystem::path& file)
	{
		return std::unique_ptr<FileHandler>();
	}
}