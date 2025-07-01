#ifndef _INTERFACE_GEOMETRY_CACHE_HPP_
#define _INTERFACE_GEOMETRY_CACHE_HPP_
#include <vector>
namespace dragon
{
	class IGeometryCache
	{
	public: 
		struct offset
		{
			int begin{ 0 };
			int end{ 0 };
			std::vector<uint32_t> indices{};
		};
	public: 
		virtual void insert(const int& id, const offset& offset) = 0;
		virtual void clear() = 0; 
	};
}
#endif // !_INTERFACE_GEOMETRY_CACHE_HPP_
