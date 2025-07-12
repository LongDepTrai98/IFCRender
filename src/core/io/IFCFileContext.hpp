#ifndef _IFC_FILE_CONTEXT_HPP_
#define _IFC_FILE_CONTEXT_HPP_
#include "IFileContext.hpp"
#include <vector>
#include <functional>
#include <unordered_set>
namespace threepp
{
	class Object3D;
	class Material;
}
namespace dragon
{
	class IFCModelCache;
	class IFCGeometryCache;
	class ItemData;
	class IFCFileContext : public IFileContext
	{
	public:
		IFCFileContext();
		~IFCFileContext();
	public:
		std::string getFileType() override;
		IFCModelCache* getModelCache();
		void handleRaycast(CustomRayCaster& RayCaster, threepp::Vector2& nor_mouse_pos) override;
		void handleHoverResult(std::shared_ptr<threepp::Mesh>& object_hover) override;
		void rebuildVisibleIndices(std::unordered_set<uint32_t> set_hides_offset);
	public:
		void initCallback();
	public:
		std::shared_ptr<std::function<void(const std::pair<int, ItemData*>&)>> m_Toggle_Component_Callback{ nullptr };
		std::shared_ptr<std::function<void(const std::vector<std::pair<int, ItemData*>>&)>> m_Toggle_Components_Callback{ nullptr };
		std::shared_ptr<std::function<void* (const int&)>> m_GetData_Item_Callback{ nullptr }; 
	private:
		std::unique_ptr<IFCModelCache> m_Model{ nullptr };
		std::shared_ptr<threepp::Material> m_Material_Hover{ nullptr };
		int m_Current_ExpressID{ -1 };
		int m_Old_ExpressID{ -1 };
	};
}
#endif // !_IFC_FILE_CONTEXT_HPP_