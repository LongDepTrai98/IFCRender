#ifndef _INTERFACE_GEOMETRY_CACHE_HPP_
#define _INTERFACE_GEOMETRY_CACHE_HPP_
namespace dragon
{
	class IGeometryCache
	{
	public: 
		struct offset
		{
			int begin; 
			int end; 
		};
	public: 
		virtual void insert(const int& id, const offset& offset) = 0;
	};
}
#endif // !_INTERFACE_GEOMETRY_CACHE_HPP_
