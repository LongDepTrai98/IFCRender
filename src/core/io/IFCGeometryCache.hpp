#ifndef _IFC_GEOMETRY_CACHE_HPP_
#define _IFC_GEOMETRY_CACHE_HPP_
#include "IGeometryCache.hpp"
#include <map>
#include <vector>
#include <unordered_map>
namespace webifc::manager {
	class ModelManager;
}
namespace dragon
{
	class IFCGeometryCache : public IGeometryCache
	{
	public:
		struct offset
		{
			int begin_vertex_offset{ 0 };
			int end_vertext_offset{ 0 };
			int begin_indices_offset{ 0 };
			int end_indices_offset{ 0 };
		};
	public:
		~IFCGeometryCache();
		IFCGeometryCache() = default;
	public:
		void clear() override;
		void setModelManager(std::shared_ptr<webifc::manager::ModelManager> modelManager, const int& modelID);
	public:
		std::unordered_map<int, std::vector<IFCGeometryCache::offset>> m_Geometry_Offset{};
	private:
		std::shared_ptr<webifc::manager::ModelManager> m_ModelManager{ nullptr };
		int m_modelID{ -1 };
	};
}
#endif // !_IFC_GEOMETRY_CACHE_HPP_