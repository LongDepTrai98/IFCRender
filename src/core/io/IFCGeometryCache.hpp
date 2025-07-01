#ifndef _IFC_GEOMETRY_CACHE_HPP_
#define _IFC_GEOMETRY_CACHE_HPP_
#include "IGeometryCache.hpp"
#include <map>
#include <vector>
namespace dragon
{
	class IFCGeometryCache : public IGeometryCache
	{
	public: 
		~IFCGeometryCache(); 
	public: 
		void insert(const int& id, const offset& offset) override
		{
			m_Data_Offset[id].emplace_back(offset); 
		}
		void copyData(const std::map<int,std::vector<offset>>& anotherData)
		{
			m_Data_Offset = anotherData; 
		}
		void clear() override
		{
			m_Data_Offset.clear(); 
		}
		bool existExpressID(const int& expressID)
		{
			return m_Data_Offset.count(expressID) != 0; 
		}
		std::map<int, std::vector<offset>>& getDataOffset()
		{
			return m_Data_Offset; 
		}
	private:
		std::map<int, std::vector<offset>> m_Data_Offset{}; 
	};
}
#endif // !_IFC_GEOMETRY_CACHE_HPP_
