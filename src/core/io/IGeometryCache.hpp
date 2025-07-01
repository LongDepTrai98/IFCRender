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
		struct offset
		{
			int begin{ 0 };
			int end{ 0 };
			std::vector<uint32_t> indices{};
			std::shared_ptr<threepp::BufferGeometry> geometry{ nullptr }; 
		};
	public: 
		virtual void insert(const int& id, const offset& offset) = 0;
		virtual void clear() = 0; 
	};
}
#endif // !_INTERFACE_GEOMETRY_CACHE_HPP_
