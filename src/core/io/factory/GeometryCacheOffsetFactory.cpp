#include "GeometryCacheOffsetFactory.hpp"
#include "core/io/IFCGeometryCache.hpp"
namespace dragon
{
	std::unique_ptr<IGeometryCache> GeometryCacheOffsetFactory::create(TYPE type)
	{
		switch (type)
		{
		case TYPE::IFC:
		{
			return std::make_unique<IFCGeometryCache>();
			break;
		}
		default:
			break;
		}
		return nullptr;
	}
}