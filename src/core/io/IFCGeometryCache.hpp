#ifndef _IFC_GEOMETRY_CACHE_HPP_
#define _IFC_GEOMETRY_CACHE_HPP_
#include "IGeometryCache.hpp"
#include <map>
namespace dragon
{
	class IFCGeometryCache : public IGeometryCache
	{
	public: 
		void insert(const int& id, const offset& offset) override
		{
			m_Data_Offset.insert({ id,offset }); 
		}
	private:
		std::map<int, offset> m_Data_Offset{}; 
	};
}
#endif // !_IFC_GEOMETRY_CACHE_HPP_
