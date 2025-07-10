#include "FileContextFactory.hpp"
#include "core/io/IFCFileContext.hpp"
namespace dragon
{
	std::unique_ptr<IFileContext> FileContextFactory::create(type type)
	{
		switch (type)
		{
		case dragon::FileContextFactory::IFC:
		{
			return std::make_unique<IFCFileContext>();
			break;
		}
		default:
			break;
		}
		return nullptr;
	}
}