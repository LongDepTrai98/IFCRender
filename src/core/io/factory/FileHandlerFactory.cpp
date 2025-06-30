#include "FileHandlerFactory.hpp"
#include "core/io/IFCFileHandler.hpp"
#include "core/utils/StringHelper.hpp"
namespace dragon
{
	std::unique_ptr<FileHandler> FileHandlerFactory::create(const std::filesystem::path& file)
	{
		if (!std::filesystem::exists(file)) return nullptr;
		if (!std::filesystem::is_regular_file(file)) return nullptr;
		auto path_extension = file.extension();
		if (StringHelper::tolower(path_extension.string()) == ".ifc")
		{
			/*CREATE IFC HANDLER*/
			return std::make_unique<IFCFileHandler>();
		}
		return nullptr;
	}
}