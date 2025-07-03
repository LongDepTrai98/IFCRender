#ifndef _INTERFACE_GEOMETRY_CACHE_HPP_
#define _INTERFACE_GEOMETRY_CACHE_HPP_
#include <vector>
#include "memory"
namespace threepp
{
	class BufferGeometry;
}
namespace dragon
{
	class IGeometryCache
	{
	public:
		virtual void clear() = 0;
	};
}
#endif // !_INTERFACE_GEOMETRY_CACHE_HPP_