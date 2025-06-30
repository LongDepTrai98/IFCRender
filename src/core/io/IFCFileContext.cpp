#include "IFCFileContext.hpp"
#include "IFCGeometryCache.hpp"
namespace dragon
{
	IFCFileContext::IFCFileContext()
	{
	}
	IFCFileContext::~IFCFileContext()
	{
	}
	std::string IFCFileContext::getFileType()
	{
		return std::string("IFC");
	}
	IGeometryCache* IFCFileContext::getGeometryCache()
	{
		return nullptr;
	}
}