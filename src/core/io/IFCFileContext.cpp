#include "IFCFileContext.hpp"
#include "IFCGeometryCache.hpp"
#include "core/io/factory/GeometryCacheOffsetFactory.hpp"
namespace dragon
{
	IFCFileContext::IFCFileContext()
	{
		m_Geometry_Offset_Cache = std::move(GeometryCacheOffsetFactory::create(GeometryCacheOffsetFactory::TYPE::IFC)); 
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
		return m_Geometry_Offset_Cache.get(); 
	}
}