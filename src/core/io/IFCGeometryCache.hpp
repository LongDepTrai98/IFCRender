#ifndef _IFC_GEOMETRY_CACHE_HPP_
#define _IFC_GEOMETRY_CACHE_HPP_
#include "IGeometryCache.hpp"
#include <map>
#include <vector>
#include <unordered_map>
#include "threepp/core/misc.hpp"
namespace webifc::manager {
	class ModelManager;
}
namespace threepp
{
	class Object3D;
	class Material;
}
namespace dragon
{
	class IFCModelCache : public IGeometryCache
	{
	public:
		struct offset
		{
			int begin_vertex_offset{ 0 };
			int end_vertext_offset{ 0 };
			int begin_indices_offset{ 0 };
			int end_indices_offset{ 0 };
			/*DRAW RANGE BEGIN, START, MATERIAL*/
			threepp::GeometryGroup group{ 0,0,0 };
		};
	public:
		~IFCModelCache();
		IFCModelCache() = default;
	public:
		void clear() override;
		void setModelManager(std::shared_ptr<webifc::manager::ModelManager> modelManager, const int& modelID);
	public:
		std::unordered_map<int, std::vector<IFCModelCache::offset>> m_Geometry_Offset{};
		threepp::Object3D* m_Object_Model{ nullptr };
		std::vector<unsigned int> m_Object_indices{};
		std::shared_ptr<webifc::manager::ModelManager> m_ModelManager{ nullptr };
		int m_modelID{ -1 };
	};
}
#endif // !_IFC_GEOMETRY_CACHE_HPP_