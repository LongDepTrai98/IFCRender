#ifndef _IFC_FILE_CONTEXT_HPP_
#define _IFC_FILE_CONTEXT_HPP_
#include "IFileContext.hpp"
#include "threepp/math/Vector3.hpp"
#include <vector>
#include <functional>
#include <unordered_set>
namespace threepp
{
	class Object3D;
	class Material;
	class BufferGeometry;
	class Points;
	class PointsMaterial;
	class MeshBasicMaterial;
}
namespace dragon
{
	class IFCModelCache;
	class IFCGeometryCache;
	class ItemData;
	class CustomRayCaster;
	class IFCFileContext : public IFileContext
	{
	public:
		IFCFileContext();
		~IFCFileContext();
	public:
		std::string getFileType() override;
		IFCModelCache* getModelCache();
		void handleRaycast(CustomRayCaster& RayCaster, MouseState& mouse_state) override;
		void initLayerOverLay() override;
		void handleHoverResult() override;
		void rebuildVisibleIndices();
		void drawHoverLayer();
		void drawHitPointLayer();
		std::shared_ptr<threepp::Mesh> createHoverMesh();
		std::shared_ptr<threepp::Mesh> createHitPoint();
	public:
		void initCallback();
	public:
		std::shared_ptr<std::function<void(const std::pair<int, ItemData*>&)>> m_Toggle_Component_Callback{ nullptr };
		std::shared_ptr<std::function<void(const std::vector<std::pair<int, ItemData*>>&)>> m_Toggle_Components_Callback{ nullptr };
		std::shared_ptr<std::function<void* (const int&)>> m_GetData_Item_Callback{ nullptr };
		std::shared_ptr<std::function<bool(const unsigned int)>> m_Callback_Intersect{ nullptr };
	public:
		CustomRayCaster* RayCast{ nullptr };
	private:
		std::unique_ptr<IFCModelCache> m_Model{ nullptr };
		std::unordered_set<unsigned int> m_Hidden_Express_IDs;
		std::shared_ptr<threepp::Material> m_Material_Hover{ nullptr };
		std::shared_ptr<threepp::MeshBasicMaterial> m_Material_Hit_Point{ nullptr };
		std::shared_ptr<threepp::Mesh> m_Object_OverLay_Hover{ nullptr };
		std::shared_ptr<threepp::Mesh> m_Hit_Point{ nullptr };
		int m_Current_ExpressID{ -1 };
		int m_Old_ExpressID{ -1 };
		threepp::Vector3 m_Coord_HitPoint{ 0.f,0.f,0.f };
	};
}
#endif // !_IFC_FILE_CONTEXT_HPP_