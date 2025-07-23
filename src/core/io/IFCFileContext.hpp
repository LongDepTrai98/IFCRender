#ifndef _IFC_FILE_CONTEXT_HPP_
#define _IFC_FILE_CONTEXT_HPP_
#include "IFileContext.hpp"
#include "threepp/math/Vector3.hpp"
#include <vector>
#include <functional>
#include <unordered_set>
#include <optional>
namespace threepp
{
	class Object3D;
	class Material;
	class BufferGeometry;
	class Points;
	class PointsMaterial;
	class MeshBasicMaterial;
	class AxesHelper;
	class RawShaderMaterial;
	class Scene;
	class Group;
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
		void drawSelectedLayer();
		void updateCoordHitPoint();
		void updateCoordAxesHelper();
		std::shared_ptr<threepp::Mesh> createHoverMesh();
		std::shared_ptr<threepp::Mesh> createHitPoint();
		std::shared_ptr<threepp::AxesHelper> createAxesHelper();
		std::shared_ptr<threepp::Mesh> createSelectedMesh(); 
	public:
		/*CLICK EVENT*/
		void LButtonUp(EventData& data) override;
		void LButtonDown(EventData& data) override;
		void RButtonUp(EventData& data) override;
		void RButtonDown(EventData& data) override;
		void KeyDown(KeyData& data) override;
		void KeyUp(KeyData& data) override;
		void ToolBarAction(ToolBarData& data) override;
	public:
		void initCallback();
	public:
		std::shared_ptr<std::function<void(const std::pair<int, ItemData*>&)>> m_Toggle_Component_Callback{ nullptr };
		std::shared_ptr<std::function<void(const std::vector<std::pair<int, ItemData*>>&)>> m_Toggle_Components_Callback{ nullptr };
		std::shared_ptr<std::function<void* (const int&)>> m_GetData_Item_Callback{ nullptr };
		std::shared_ptr<std::function<bool(const unsigned int)>> m_Callback_Intersect{ nullptr };
		std::function<void(const std::vector<std::shared_ptr<threepp::Mesh>>& meshes)> m_Add_Object_CallBack{ nullptr };
		std::function<void(const std::vector<std::shared_ptr<threepp::BufferGeometry>>& geometries)> m_Add_Object_DrawDepth_CallBack{ nullptr };
	public:
		CustomRayCaster* RayCast{ nullptr };
		std::shared_ptr<threepp::Group> m_OverLay_Group{ nullptr };
	private:
		std::unique_ptr<IFCModelCache> m_Model{ nullptr };
		std::unordered_set<unsigned int> m_Hidden_Express_IDs;
		std::unordered_map<unsigned int, std::shared_ptr<threepp::Mesh>> m_Selected_Entites;
		/*MESH*/
		std::shared_ptr<threepp::Mesh> m_Object_OverLay_Hover{ nullptr };
		std::shared_ptr<threepp::Mesh> m_Hit_Point{ nullptr };
		std::shared_ptr<threepp::AxesHelper> m_Axes_Helper{ nullptr };
		std::shared_ptr<threepp::Mesh> m_Object_Selected{ nullptr }; 
		/*MATERIAL*/
		std::shared_ptr<threepp::MeshBasicMaterial> m_Material_Hit_Point{ nullptr };
		std::shared_ptr<threepp::Material> m_Material_Hover{ nullptr };
		std::shared_ptr<threepp::MeshBasicMaterial> m_Basic_Material{ nullptr };
		std::shared_ptr<threepp::MeshBasicMaterial> m_Selected_Material{ nullptr };
		std::optional<int> m_Current_ExpressID;
		std::optional<int> m_Old_ExpressID{ -1 };
		std::optional<threepp::Vector3> m_Coord_HitPoint;
		std::optional<threepp::Vector3> m_Coord_HitPoint_Clicked;
		std::optional<threepp::Vector3> m_Center_Point;
		bool m_bIsSelectPivotMode{ false };
		bool m_bIsHoverMode{ false };
	};
}
#endif // !_IFC_FILE_CONTEXT_HPP_