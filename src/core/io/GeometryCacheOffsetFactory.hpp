#ifndef _GEOMETRY_CACHE_OFFSET_FACTORY_HPP_
#define _GEOMETRY_CACHE_OFFSET_FACTORY_HPP_
#include <memory>
namespace dragon
{
	class IGeometryCache;
	class GeometryCacheOffsetFactory
	{
	public: 
		enum TYPE
		{
			IFC,
		};
	public: 
		static std::unique_ptr<IGeometryCache> create(TYPE type);
	private: 
		GeometryCacheOffsetFactory() = default; 
	};
}
#endif // !_GEOMETRY_CACHE_OFFSET_FACTORY_HPP_
