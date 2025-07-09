#ifndef _IFC_FILE_CONTEXT_HPP_
#define _IFC_FILE_CONTEXT_HPP_
#include "IFileContext.hpp"
#include <vector>
#include <functional>
namespace threepp
{
	class Object3D;
	class Material;
}
namespace dragon
{
	class IGeometryCache;
	class IFCGeometryCache;
	class ItemData; 
	class IFCFileContext : public IFileContext
	{
	public:
		IFCFileContext();
		~IFCFileContext();
	public:
		std::string getFileType() override;
		IGeometryCache* getGeometryCache() override;
		void handleRaycast(CustomRayCaster& RayCaster, threepp::Vector2& nor_mouse_pos) override;
		void handleHoverResult(std::shared_ptr<threepp::Mesh>& object_hover) override;
		void hideParts(const std::vector<uint32_t>& parts); 
	public:
		void setRootObject(threepp::Object3D* root_mesh);
		void initCallback(); 
	public: 
		std::shared_ptr<std::function<void(const std::pair<int, ItemData*>&)>> m_Toggle_Component_Callback{ nullptr };
		std::shared_ptr<std::function<void(const std::vector<std::pair<int, ItemData*>>&)>> m_Toggle_Components_Callback{ nullptr };
	private:
		std::unique_ptr<IFCGeometryCache> m_Geometry_Offset_Cache{ nullptr };
		std::vector<threepp::Object3D*> m_Children_Objects{ nullptr };
		std::shared_ptr<threepp::Material> m_Material_Hover{ nullptr };
		int m_Current_ExpressID{ -1 };
		int m_Old_ExpressID{ -1 };
	};
}
#endif // !_IFC_FILE_CONTEXT_HPP_