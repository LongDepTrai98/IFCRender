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
			int end_vertex_offset{ 0 };
			int begin_indices_offset{ 0 };
			int end_indices_offset{ 0 };
			/*DRAW RANGE BEGIN, START, MATERIAL*/
			int material_index{ 0 }; 
			int state{ 0 };
			std::shared_ptr<threepp::BufferGeometry> buffGeo{ nullptr }; 
		};
		struct element
		{
			/*AUTO SHOW*/
			int state{ 1 }; 
			std::vector<IFCModelCache::offset> offsets{}; 
		};
	public:
		~IFCModelCache();
		IFCModelCache() = default;
	public:
		void clear() override;
		void setModelManager(std::shared_ptr<webifc::manager::ModelManager> modelManager, const int& modelID);
	public:
		std::unordered_map<int, element> m_Geometry_Offset{};
		threepp::Object3D* m_Object_Model{ nullptr };
		std::vector<unsigned int> m_Object_Indices{};
		std::shared_ptr<webifc::manager::ModelManager> m_ModelManager{ nullptr };
		std::vector<std::shared_ptr<threepp::Material>> m_Object_Materials{ nullptr }; 
		int m_modelID{ -1 };
	};
}
#endif // !_IFC_GEOMETRY_CACHE_HPP_